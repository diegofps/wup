#ifndef THREADS_HPP
#define THREADS_HPP

#include <wup/common/generic.hpp>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>

namespace wup
{

using std::thread;
using std::mutex;

class Semaphore
{
public:

    Semaphore() :
        _counter(0)
    {
        _mutex2.lock();
    }

    Semaphore(const int counter) :
        _counter(counter)
    {
        if (_counter <= 0)
            _mutex2.lock();
    }

    void acquire()
    {
        _mutex2.lock();
        _mutex1.lock();

        --_counter;

        if (_counter > 0)
            _mutex2.unlock();

        _mutex1.unlock();
    }

    void release()
    {
        _mutex1.lock();
        ++_counter;

        if (_counter == 1)
            _mutex2.unlock();

        _mutex1.unlock();
    }

    void add(const int n)
    {
        _mutex1.lock();

        bool wasLocked = _counter <= 0;
        _counter += n;

        if (_counter <= 0)
        {
            if (!wasLocked)
                _mutex2.lock();
        }
        else
        {
            if (wasLocked)
                _mutex2.unlock();
        }

        _mutex1.unlock();
    }

    int
    value()
    {
        return _counter;
    }

private:

    mutex _mutex1;
    mutex _mutex2;
    int _counter;

};

class Barrier
{
public:

    Barrier(const int health) :
        sem(1 - health)
    {

    }

    void hit()
    {
        sem.release();
    }

private:

    Semaphore sem;

};

template <typename T>
class Pipe {
public:

    Pipe() : _size(10), _mem(new T[_size]), _posWrite(0), _posRead(0),
            _semRead(0), _semWrite(_size) { }

    Pipe(const int size) : _size(size), _mem(new T[_size]), _posWrite(0),
            _posRead(0), _semRead(0), _semWrite(_size) { }

    ~Pipe()
    {
        delete [] _mem;
    }

    Pipe &send(const T &data)
    {
        _semWrite.acquire();
            m.lock();
            _mem[_posWrite] = data;
            _posWrite = (_posWrite + 1) % _size;
            m.unlock();
        _semRead.release();

        return *this;
    }

    T get()
    {
        _semRead.acquire();
            m.lock();
            T data = _mem[_posRead];
            _posRead = (_posRead + 1) % _size;
            m.unlock();
        _semWrite.release();

        return data;
    }

private:

    int _size;
    T *_mem;
    int _posWrite;
    int _posRead;
    Semaphore _semRead;
    Semaphore _semWrite;
    mutex m;

};


template <typename T>
class WeightedPipe
{
public:

    WeightedPipe(std::function<bool(const T &, const T &)> & func) :
        queue(func),
        _semRead(0)
    {

    }

    WeightedPipe & send(const T & data)
    {
        m.lock();
            queue.push(data);
            _semRead.release();
        m.unlock();

        return *this;
    }

    T get()
    {
        _semRead.acquire();
        m.lock();
        T tmp = queue.top();
        queue.pop();
        m.unlock();
        return tmp;
    }

private:

    priority_queue<T, std::vector<T>, std::function<bool(const T &, const T &)>> queue;
    Semaphore _semRead;
    mutex m;

};

template <typename F>
void parallel_thread(Pipe<int> * const p, F f, const int tid)
{
    int jobId = p->get();
    while(jobId >= 0)
    {
        f(tid, jobId);
        jobId = p->get();
    }
}

// Runs a Pool of jobs in parallel
//
// numThreads: Number of threads. If 0 we will use thread::hardware_concurrency()
// jobs: The number of jobs to run in parallel
// F: void (*)(const int threadId, const int jobId)
template <typename F>
void parallel(uint threads, const size_t jobs, F f)
{
    if (threads == 0)
        threads = thread::hardware_concurrency();

    Pipe<int> p;
    thread *pool = new thread[threads];

    for (uint t=0;t!=threads;++t) {
        auto func = parallel_thread<F>;
        pool[t] = thread(func, &p, f, t);
    }

    for (size_t jobId=0;jobId!=jobs;++jobId)
        p.send(int(jobId));

    for (uint t=0;t!=threads;++t)
        p.send(-1);

    for (uint t=0;t!=threads;++t)
        pool[t].join();

    delete [] pool;
}


template <typename F>
void parallel_blocks_thread(Pipe<int> * const p, F f, const int tid, const size_t jobs, const size_t blockSize)
{
    int blockId = p->get();
    while(blockId >= 0)
    {
        const size_t first = blockId * blockSize;
        const size_t last  = wmin(first + blockSize, jobs);

        f(tid, blockId, first, last);
        blockId = p->get();
    }
}


template <typename F>
void parallelBlocks(const uint threads, const size_t jobs, const size_t blockSize, F f)
{
    const size_t numBlocks = jobs % blockSize
            ? jobs / blockSize + 1
            : jobs / blockSize;

    Pipe<int> p;
    thread *pool = new thread[threads];

    for (uint t=0;t!=threads;++t) {
        auto func = parallel_blocks_thread<F>;
        pool[t] = thread(func, &p, f, t, jobs, blockSize);
    }

    for (size_t blockId=0;blockId!=numBlocks;++blockId)
        p.send(int(blockId));

    for (uint t=0;t!=threads;++t)
        p.send(-1);

    for (uint t=0;t!=threads;++t)
        pool[t].join();

    delete [] pool;
}

}

#endif // THREADS_HPP
