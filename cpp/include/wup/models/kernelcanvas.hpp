/*
 * kernelcanvas.hpp
 *
 *  Created on: 12/12/2015
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_MODELS_KERNELCANVAS_HPP_
#define INCLUDE_WUP_MODELS_KERNELCANVAS_HPP_

#include <wup/models/kernelgens.hpp>
#include <cstdlib>

namespace wup {

class EuclidianKernels {
public:

    EuclidianKernels (const int numKernels, const int dims,
    		const double act, double ** kernels) :
        _numKernels(numKernels),
		_dims(dims),
		_tmp(new double[dims]),
		_kernels(kernels),
		_selections(NULL),
		_boxes(NULL)
    {
    	if (_kernels == NULL)
	        kernelgens::createRandomKernels(_dims, _numKernels, _kernels);

    	_k = int(ceil(_numKernels * act));
    	_boxes = new BOX[_numKernels];
    	_selections = new int[_k];
    }

    virtual
	~EuclidianKernels ()
    {
        for (int i=0;i<_numKernels;++i)
            delete _kernels[i];

        delete [] _selections;
        delete [] _kernels;
        delete [] _boxes;
        delete [] _tmp;
    }

    const int *
	select(const double * const pattern)
    {
        for (int i=0;i<_dims;++i)
            _tmp[i] = pattern[i];

        for (int i=0;i<_numKernels;++i) {
            _boxes[i].w = -wup::sdistance(_tmp, _kernels[i], _dims);
            _boxes[i].id = i;
        }

        wup::halfqsort(_boxes, 0, _numKernels-1, _k-1);

        for (int i=0;i< _k; ++i)
            _selections[i] = _boxes[i].id;

        return _selections;
    }

    int k() const
    { return _k; }

    int numKernels() const
    { return _numKernels; }

private:
    int _numKernels;
    const int _dims;
    double * _tmp;
    double ** _kernels;
    int * _selections;
    int _k;
	BOX *_boxes;
};

template <typename KernelSpace=EuclidianKernels>
class KernelCanvas {
public:

    KernelCanvas(const int inputs, const int numKernels, const double act,
            const int term_bits, double ** kernels) :
        _term_bits(term_bits),
        _kernelSpace(numKernels, inputs, act, kernels),
        _outputFreq(new int[_kernelSpace.numKernels()]),
        _outputBits(new int[_kernelSpace.numKernels() * term_bits])
    { }

    ~KernelCanvas()
    {
        delete [] _outputFreq;
        delete [] _outputBits;
    }

    void clear()
    {
        for (int i=0;i<_kernelSpace.numKernels();++i)
            _outputFreq[i] = 0;
    }

    void read(const double * pattern)
    {
        const int * const ids = _kernelSpace.select( pattern );
        for (int i=0; i<_kernelSpace.k(); ++i)
            _outputFreq[ids[i]] = 1;
    }

    int * binary_output()
    {
        int offset = -1;
        for (int i=0;i<_kernelSpace.numKernels();++i)
			if (_outputFreq[i] == 1)
				for (int j=0;j<_term_bits;++j)
					_outputBits[++offset] = 1;
			else
				for (int j=0;j<_term_bits;++j)
					_outputBits[++offset] = 0;
        return _outputBits;
    }

    double * real_output()
    { return _outputFreq; }

    int binary_outputs() const
    { return _kernelSpace.numKernels() * _term_bits; }

    int real_outputs() const
    { return _kernelSpace.numKernels(); }

private:
    int _term_bits;
    KernelSpace _kernelSpace;
    int * _outputFreq;
    int * _outputBits;
};

} /* wup */

#endif /* INCLUDE_WUP_MODELS_KERNELCANVAS_HPP_ */
