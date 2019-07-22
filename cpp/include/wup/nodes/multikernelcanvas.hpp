#ifndef MULTIKERNELCANVAS_HPP
#define MULTIKERNELCANVAS_HPP

/*
 * multikernelcanvas.hpp
 *
 *  Created on: 12/01/2018
 *      Author: diegofps
 */

#include <wup/nodes/node.hpp>
#include <wup/models/kernelcanvas.hpp>
#include <cmath>

namespace wup {

namespace node {

class MultiKernelCanvas : public Node {

    typedef wup::KernelCanvas<EuclidianKernels> KC;

private:

    std::vector<KC*> _kcs;

public:

    MultiKernelCanvas(Node * const parent, IntReader & reader) :
            Node(parent, reader),
            _kcs()
    {
        uint numCanvas = reader.getUInt32();

        for (uint i=0; i!=numCanvas; ++i)
            _kcs.push_back(new KC(reader));
    }

    MultiKernelCanvas(Node * const parent, const uint numKernels, const uint kernelDims,
                      const double activation, const uint bits) :
        Node(parent),
        _kcs()
    {
        const uint size = parent->output().size();
        uint i = 0;

        while(i != size)
        {
            const uint dims = min(kernelDims, size-i);
            _kcs.push_back(new KC(kernelDims, numKernels, activation, bits));
            i += dims;
        }
    }

    ~MultiKernelCanvas()
    {
        for (KC * kc : _kcs)
            delete kc;
    }

    virtual void
    onClear()
    {
        for (KC * kc : _kcs)
            kc->clear();
    }

    virtual void
    onDigest(const Feature & input)
    {
        uint current = 0;

        for (KC * kc : _kcs)
        {
            const double & tmp = input[current];
            kc->read(&tmp);
            current += kc->kernelSpace().dims();
        }
    }

    virtual void
    toPattern(int * dst)
    {
        for (KC * kc : _kcs)
        {
            const ulong length = (ulong)(kc->binary_output_size());
            memcpy(dst, kc->binary_output(), sizeof(int) * length);
            dst += length;
        }
    }

    virtual uint patternSize()
    {
        uint size = 0;

        for (KC * kc : _kcs)
            size += kc->binary_output_size();

        return size;
    }


    virtual void onExport(IntWriter & writer)
    {
        writer.put(int(_kcs.size()));

        for (KC * kc : _kcs)
            kc->exportTo(writer);
    }

    bool operator ==(node::MultiKernelCanvas const& other) const
    {
        if (_kcs.size() != other._kcs.size())
            return false;

        for (uint i=0;i!=_kcs.size();++i)
            if (!(_kcs[i]->operator==(*other._kcs[i])))
                return false;

        return true;
    }

};

} /* node */

} /* wup */

#endif // MULTIKERNELCANVAS_HPP
