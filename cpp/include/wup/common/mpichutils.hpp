#ifndef MPICHUTILS_HPP
#define MPICHUTILS_HPP

#include <wup/common/params.hpp>
#include <wup/common/bundle.hpp>

#include <functional>
#include <unordered_map>
#include <mpich/mpi.h>
#include <cstdint>
#include <thread>

namespace wup
{

class MPIServer
{
private:

    std::unordered_map<int, std::function<void(MPI_Status&, uint8_t*, const int32_t)>> callbacks;

    bool _continue;

    std::thread * t;

    int delay;

public:

    MPIServer(Params & params) :
        t(nullptr)
    {
        delay = params.getInt("delay", 5);
    }

    ~MPIServer()
    {
        stop();
    }

    template <typename T>
    void
    route(const int r, T callback)
    {
        auto it = callbacks.find(r);

        if (it == callbacks.end())
            callbacks[r] = callback;
        else
            error("Route ", r, " is being registered twice");
    }

    void
    asyncRun()
    {
        stop();

        t = new thread([&]() {
            run();
        });
    }

    void
    run()
    {
        vector<uint8_t> buffer;
        MPI_Status status;
        int count;
        int flag;

        _continue = true;

        while (_continue)
        {
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

            if (flag)
            {
                MPI_Get_count(&status, MPI_BYTE, &count);
                const size_t required = static_cast<size_t>(count);

                if (buffer.size() < required)
                    buffer.resize(required);

                MPI_Recv(buffer.data(), count, MPI_BYTE, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);

                auto it = callbacks.find(status.MPI_TAG);

                if (it == callbacks.end())
                    warn("Received an invalid route from client: ", status.MPI_TAG);
                else
                    it->second(status, buffer.data(), count);
            }
            else
            {
                this_thread::sleep_for(chrono::milliseconds(delay));
            }
        }
    }

    // Not working, but should be more efficient as it has no
    // pooling, just a blocking MPI_Probe.
    void
    run2()
    {
        Bundle<uint8_t> buffer;
//        vector<uint8_t> buffer;
        MPI_Status status;
        int count;

        _continue = true;

        while (_continue)
        {
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            MPI_Get_count(&status, MPI_BYTE, &count);

            const size_t required = static_cast<size_t>(count);

            if (buffer.size() < required)
                buffer.reshape(required);

            //print(buffer.size(), " ", count, " ", status.MPI_SOURCE, " ", status.MPI_TAG, " ", status.MPI_ERROR);
            MPI_Recv(buffer.data(), count, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            auto it = callbacks.find(status.MPI_TAG);

            if (it == callbacks.end())
                warn("Received an invalid route from client: ", status.MPI_TAG);
            else
                it->second(status, buffer.data(), count);
        }
    }

    void
    stop()
    {
        _continue = false;
        if (t != nullptr)
        {
            t->join();
            delete t;
            t = nullptr;
        }
    }

    bool
    isRunning()
    {
        return _continue;
    }

};

#define ADD_ROUTE(server, route_name, callback) \
    server.route(route_name, [&](MPI_Status & status, uint8_t * data, const int32_t size)\
    {\
        callback(status, data, size);\
    });

} /* wup */

#endif // MPICHUTILS_HPP
