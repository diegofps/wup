/*
 * kernelcanvas.hpp
 *
 *  Created on: 12/12/2015
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_KERNELCANVAS_HPP_
#define INCLUDE_WUP_NODES_KERNELCANVAS_HPP_

#include <wup/nodes/node.hpp>
#include <wup/models/kernelcanvas.hpp>

//#include <wup/models/oldkernelcanvas.hpp>

namespace wup {

namespace node {

class KernelCanvas : public Node {
public:

    KernelCanvas(Node * const parent, IntReader & reader) :
            Node(parent, reader),
            _kc(reader)
    {

    }

    KernelCanvas(Node * const parent, const uint numKernels,
                 const double activation, const uint termBits,
                 double ** kernels=NULL) :
        Node(parent),
        _kc(parent->output().size(), numKernels, activation, termBits, kernels)
    {

    }

    virtual void
    onClear()
    {
        _kc.clear();
    }

    virtual
    void onStart(const int & /*sampleId*/)
    {
        //_kc.clearInk();
    }

    virtual void
    onDigest(const Feature & input)
    {
        _kc.read(input.data());
    }

    virtual void onFinish()
    {

    }

    virtual void
    toPattern(int * dst)
    {
        memcpy(dst, _kc.binary_output(),
                sizeof(int) * _kc.binary_output_size());
//        memcpy(dst, _kc.timestamp_output(),
//                sizeof(int) * _kc.timestap_output_size());
    }

    virtual uint patternSize()
    {
        return _kc.binary_output_size();
//        return _kc.timestap_output_size();
    }


    virtual void onExport(IntWriter & writer)
    {
        _kc.exportTo(writer);
    }

    bool operator ==(node::KernelCanvas const& other) const
    {
        return _kc == other._kc;
    }

private:

    wup::KernelCanvas<EuclidianKernels> _kc;
    //legacy::KernelCanvas _kc;

};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_KERNELCANVAS_HPP_ */
