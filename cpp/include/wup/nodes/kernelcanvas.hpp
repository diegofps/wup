/*
 * kernelcanvas.hpp
 *
 *  Created on: 12/12/2015
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_KERNELCANVAS_HPP_
#define INCLUDE_WUP_NODES_KERNELCANVAS_HPP_

#include <wup/models/kernelcanvas.hpp>

namespace wup {

namespace node {

class KernelCanvas : public Node {
public:

    KernelCanvas(Node * const parent, wup::sbreader<double> &reader) :
            Node(parent),
            _kc(reader)
    { }

    KernelCanvas(Node * const parent, const int kernels,
			const double activation, const int bits) :
		Node(parent),
		_kc(parent->outputLength(), kernels, activation, bits, NULL)
	{ }

    virtual void onStart()
    {
    	_kc.clear();
    }

    virtual void onDigest(const Feature & input)
    {
    	_kc.read(input.data());
    }

    virtual void binaryOutput(int * dst)
    {
    	memcpy(dst, _kc.binary_output(),
    			sizeof(int)*_kc.binary_outputs());
    }

    virtual int binaryOutputLength()
    { return _kc.binary_outputs(); }

    virtual void exportTo(wup::sbwriter<double> &writer) {
        _kc.exportTo(writer);
    }

 	bool operator ==(node::KernelCanvas const& other) const {
 		return _kc == other._kc;
 	}

private:

	wup::KernelCanvas<EuclidianKernels> _kc;

};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_KERNELCANVAS_HPP_ */
