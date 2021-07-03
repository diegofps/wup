#ifndef PRIORITYFLOW_HPP
#define PRIORITYFLOW_HPP

#include <wup/common/threads.hpp>
#include <wup/common/bundle.hpp>

#include <mutex>
#include <vector>
#include <queue>
#include <functional>

namespace wup
{

class SharedCounter
{
private:

    int _counter;
    std::mutex m;

public:

    SharedCounter() :
        _counter(0) { }

    SharedCounter(const int counter) :
        _counter(counter) { }

    SharedCounter(const uint counter) :
        _counter(int(counter)) { }

    void
    set(const int v)
    {
        m.lock();
        _counter = v;
        m.unlock();
    }

    int
    inc()
    {
        m.lock();
        int result = ++_counter;
        m.unlock();
        return result;
    }

    int
    inc(const int v)
    {
        m.lock();
        _counter += v;
        int result = _counter;
        m.unlock();
        return result;
    }

    int
    dec()
    {
        m.lock();
        int result = --_counter;
        m.unlock();
        return result;
    }

    int
    dec(const int v)
    {
        m.lock();
        _counter -= v;
        const int result = _counter;
        m.unlock();
        return result;
    }

};

class CommonJob
{
public:

    uint tid; // thread id
    int nid; // node id
    uint first; // first element in block (included)
    uint last; // last element in block (excluded)

public:

    CommonJob() :
        tid(0), nid(-1), first(0), last(0) { }

    CommonJob(const int nid) :
        tid(0), nid(nid), first(0), last(0) { }

    CommonJob(const int nid, const uint first, const uint last) :
        nid(nid), first(first), last(last) { }

};

std::ostream &
operator<<(std::ostream & o,
           CommonJob const & j)
{
    o << "(tid:" << j.tid << ", nid:" << j.nid << ", first:" << j.first << ", last:" << j.last << ")";
    return o;
}


bool
job_comparer(CommonJob const & l,
             CommonJob const & r)
{
    if (l.nid != r.nid)
        return l.nid < r.nid;

    else
        return l.first > r.first;
}


template <typename JOB_TYPE>
class Node
{
public:

    int nid;

    std::function<void(JOB_TYPE &)> body;

public:

    Node() : nid(-1) { }

    Node(int const nid) : nid(nid) { }

    Node(Node const & other)
    {
        warn("Copy constructor of Node is generaly a bad idea");
        body = other.body;
        nid = other.nid;
    }
};


template <typename J>
void
priority_flow_thread(WeightedPipe<J> * pipe,
                  std::vector<Node<J>*> * nodes,
                  uint32_t const tid)
{
    J job = pipe->get();
    job.tid = tid;
//    print("tid", tid, "job nid", job.nid, job.model, job.first, job.last);

    while(job.nid != -1)
    {
        (*nodes)[job.nid]->body(job);
        job = pipe->get();
        job.tid = tid;
    }
}


template <typename JOB_TYPE>
class PriorityFlow
{
private:

    std::vector<Node<JOB_TYPE>*> nodes;
    Semaphore sem;
    WeightedPipe<JOB_TYPE> pipe;
    std::thread * pool;
    uint _threads;

    void
    init()
    {
        if (_threads == 0)
            _threads = std::thread::hardware_concurrency();

        pool = new std::thread[_threads];

        for (uint32_t t=0;t!=_threads;++t)
        {
            auto f = priority_flow_thread<JOB_TYPE>;
            pool[t] = std::thread(f, &pipe, &nodes, t);
        }
    }

public:

    PriorityFlow() :
        pipe(job_comparer),
        _threads(std::thread::hardware_concurrency())
    {
        init();
    }

    PriorityFlow(uint32_t const threads) :
        pipe(job_comparer),
        _threads(threads)
    {
        init();
    }

    PriorityFlow(uint32_t const threads,
                 std::function<bool(const JOB_TYPE &, const JOB_TYPE &)> comparator) :

        pipe(comparator),
        _threads(threads)
    {
        init();
    }

    ~PriorityFlow()
    {
        JOB_TYPE finish_cmd;
        finish_cmd.nid = -1;

        for (uint t=0;t!=_threads;++t)
            schedule(finish_cmd);

        for (uint t=0;t!=_threads;++t)
            pool[t].join();

        for (auto node : nodes)
            delete node;

        delete [] pool;
    }

    Node<JOB_TYPE> &
    createNode(std::function<void(JOB_TYPE &)> func)
    {
        nodes.push_back(new Node<JOB_TYPE>(nodes.size()));
        nodes.back()->body = func;
        return *nodes.back();
    }

    Node<JOB_TYPE> &
    createNode()
    {
        nodes.push_back(new Node<JOB_TYPE>(nodes.size()));
        return *nodes.back();
    }

    void
    schedule(JOB_TYPE const & job)
    {
        pipe.send(job);
    }

    int
    scheduleMany(JOB_TYPE job,
                 uint64_t const jobsPerThread,
                 uint64_t const samples)
    {
        uint64_t first = 0;
        int n = 0;

        while (first + jobsPerThread <= samples)
        {
            job.first = first;
            job.last = first + jobsPerThread;
            schedule(job);
            first += jobsPerThread;
            ++n;
        }

        if (first != samples)
        {
            job.first = first;
            job.last = samples;
            schedule(job);
            ++n;
        }

        return n;
    }

    int
    scheduleMany(int32_t const nodeId,
                 uint64_t const jobsPerThread,
                 uint64_t const samples)
    {
        JOB_TYPE tmp;
        tmp.nid = nodeId;
        return scheduleMany(tmp, jobsPerThread, samples);
    }

    void
    release()
    {
        sem.release();
    }

    void
    release(const int n)
    {
        sem.add(n);
    }

    void
    wait(const int n)
    {
        sem.add(1 - n);
        sem.acquire();
    }

};


struct Resource
{
    uint const id;
    std::vector<uint> jobs;
    uint capacity;

    Resource(uint const id,
             uint capacity) :

        id(id),
        capacity(capacity)
    {
        jobs.reserve(capacity);
    }

};


template <typename RESOURCE>
class ResourceManager
{
private:

    Bundle<uint32_t> buffer;
    std::vector<RESOURCE*> resources;
    std::function<void(RESOURCE & r)> _scheduler;
    uint32_t _pos;
    std::mutex m;

public:

    ResourceManager(uint32_t const maxJobs,
                    std::vector<RESOURCE*> & res,
                    std::function<void(RESOURCE & r)> scheduler) :

        buffer(maxJobs/* - res[0]->jobs.size() * res.size()*/),
        resources(res),
        _scheduler(scheduler),
        _pos(0)
    {

    }

    void
    addJobs(uint32_t const first,
            uint32_t const last)
    {
        uint32_t const len = last - first;

        m.lock();

        if (resources.size()/* && len >= resources[0]->jobs.size()*/)
        {
            RESOURCE & r = *resources.back();
            resources.pop_back();

            uint32_t const middle = first + min(r.capacity, len);

            r.jobs.clear();

            for (uint32_t i=first;i!=middle;++i)
                r.jobs.push_back(i);

            for (uint32_t i=middle;i!=last;++i)
                buffer(_pos++) = i;

            _scheduler(r);
        }
        else
        {
            for (uint32_t i=first;i!=last;++i)
                buffer(_pos++) = i;
        }

        m.unlock();
    }

    bool
    refill(RESOURCE & r)
    {
        uint32_t const empty = r.capacity - r.jobs.size();

        m.lock();

        if (r.capacity != r.jobs.size())
        {
            if (_pos == 0 && r.jobs.size() == 0)
            {
                // Keep this for now, this is a real warning
                //warn("Buffer was empty and the jobs are done, releasing resource");
                resources.push_back(&r);
                m.unlock();

                return false;
            }
            else
            {
                uint32_t first = empty >= _pos ? 0 : _pos - empty;
                uint32_t last = _pos;

                r.jobs.insert(r.jobs.end(), &buffer(first), &buffer(last));

                _pos = first;
            }
        }

        m.unlock();

//        warn("Calling scheduler, buffer size is now ", _pos);
        _scheduler(r);

        return true;
    }

};

} /* wup */

#endif // PRIORITYFLOW_HPP
