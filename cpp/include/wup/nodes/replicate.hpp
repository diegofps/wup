#ifndef __WUP__NODE__REPLICATE_HPP
#define __WUP__NODE__REPLICATE_HPP

#include "node.hpp"

namespace wup {

namespace node {

class Replicate : public Node {
public:
    
    Replicate(Node * const parent, const int times) :
        Node(parent, parent->output().size() * times),
        _times(times),
        _mem(new double[parent->output().size()]),
        _current(0)
    {
        if (times < 1)
            throw new WUPException(cat("Can't replicate ", times, " times"));
    }

    Replicate(const Replicate & other) :
        Node(other),
        _times(other._times),
        _mem(new double[other.output().size()]),
        _current(other._current)
    {
        memcpy(_mem, other._mem, sizeof(double) * other.output().size());
    }

    Replicate(Node * const parent, ireader & reader) :
        Node(parent, reader),
        _times(reader.get()),
        _mem(new double[parent->output().size()]),
        _current(reader.get())
    {

    }

    virtual
    void onExport(iwriter & writer)
    {
        writer.put(_times);
        writer.put(_current);
    }

    virtual ~Replicate()
    {
        delete [] _mem;
    }

    virtual void
    onStart()
    {
        output() = 0.0;
    }

    virtual void
    onDigest(const Feature & input)
    {
        Feature & out = output();
        const int length = input.size();

        for (int k=_times-1;k>0;--k) {
            const int current  = k * length;
            const int previous = (k-1) * length;

            memcpy(out.data() + current, out.data() + previous,
                    sizeof(double) * length);
        }

        memcpy(out.data(), input.data(), sizeof(double) * length);

        publish(out);
    }

    virtual void onFinish()
    {

    }

    virtual void toPattern(int * dst)
    {
        for (uint i=0;i<output().size();++i)
            dst[i] = output()[i] >= 0.5 ? 1 : 0;
    }

    virtual int
    patternSize()
    {
        return output().size();
    }
    
private:

    int _times;
    
    double * _mem;

    int _current;

};

} /* node */

} /* wup */

#endif
