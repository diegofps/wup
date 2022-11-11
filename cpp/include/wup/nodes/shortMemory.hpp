#ifndef __WUP__NODE__SHORTMEMORY_HPP
#define __WUP__NODE__SHORTMEMORY_HPP

#include "node.hpp"

namespace wup {

namespace node {

class ShortMemory : public Node {
public:
    
    ShortMemory(Node * const parent, const uint times) :
        Node(parent, parent->output().size() * times),
        _times(times),
        _mem(new double[parent->output().size()]),
        _current(0)
    {
        if (times < 1)
            throw WUPException(cat("Can't replicate ", times, " times"));
    }

    ShortMemory(Node * const parent, IntReader & reader) :
        Node(parent, reader),
        _times(reader.getUInt32()),
        _mem(new double[parent->output().size()]),
        _current(reader.get())
    {

    }

    ShortMemory(const ShortMemory & other) :
        Node(other),
        _times(other._times),
        _mem(new double[other.output().size()]),
        _current(other._current)
    {
        memcpy(_mem, other._mem, sizeof(double) * other.output().size());
    }

    virtual ~ShortMemory()
    {
        delete [] _mem;
    }

    virtual
    void onExport(IntWriter & writer)
    {
        writer.putUInt32(_times);
        writer.put(_current);
    }

    virtual
    void onStart(const int & /*sampleId*/)
    {
        output() = 0.0;
    }

    virtual void
    onDigest(const Feature & input)
    {
        Feature & out = output();
        const uint length = input.size();

        for (uint k=_times-1; k!=0; --k) {
            const uint current  = k * length;
            const uint previous = (k-1) * length;

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

    virtual uint
    patternSize()
    {
        return output().size();
    }
    
private:

    uint _times;
    
    double * _mem;

    int _current;

};

} /* node */

} /* wup */

#endif
