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
#include <wup/common/math.hpp>
#include <cmath>

namespace wup {

namespace node {

class MultiKernelCanvas : public Node {

    typedef wup::KernelCanvas<EuclideanKernelSpace> KC;

private:

    std::vector<KC*> _kcs;

public:

    MultiKernelCanvas(
        Node * const parent, 
        IntReader & reader) :

        Node(parent, reader),
        _kcs()
    {
        uint numCanvas = reader.getUInt32();

        for (uint i=0; i!=numCanvas; ++i)
            _kcs.push_back(new KC(reader));
    }

    MultiKernelCanvas(
        Node * const parent, 
        const uint numKernels, 
        const uint kernelDims, 
        const double activation, 
        const uint bits) :

        Node(parent),
        _kcs()
    {
        const uint size = parent->output().size();
        uint i = 0;

        while(i != size)
        {
            uint const numDims = math::min(kernelDims, size-i);

            wup::Bundle<double> dimRanges(2,numDims);

            for (int j=0;j!=dimRanges.cols();++j)
            {
                dimRanges(0,j) = 0.0;
                dimRanges(1,j) = 1.0;
            }

            wup::Bundle<double> kernels;
            wup::generate::randomKernels(numKernels, dimRanges, kernels);

            _kcs.push_back(new KC(activation, bits, kernels));
            i += numDims;
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
            const unsigned long length = (unsigned long)(kc->binary_output_size());
            memcpy(dst, kc->binary_output().data(), sizeof(int) * length);
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
