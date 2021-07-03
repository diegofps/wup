/*
 * kernelcanvas.hpp
 *
 *  Created on: 12/12/2015
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_MODELS_KERNELCANVAS_HPP_
#define INCLUDE_WUP_MODELS_KERNELCANVAS_HPP_

#include <cstdlib>

#include <wup/models/kernelgens.hpp>
#include <wup/common/exceptions.hpp>
#include <wup/common/sbio.hpp>
#include <wup/common/generic.hpp>

namespace wup {

class EuclidianKernels
{
public:

    EuclidianKernels (IntReader & reader) :
            _numKernels(0),
            _dims(0),
            _tmp(nullptr),
            _kernels(nullptr),
            _selections(nullptr),
            _boxes(nullptr)
    {
    	if (reader.get() != -1)
    		throw WUPException("Invalid file");

        kernelgens::importKernels(reader, _dims, _numKernels, _kernels);
        _k = reader.get();

        if (reader.get() != -1)
		throw WUPException("Invalid file");

        _tmp = new double[_dims];
        _boxes = new BOX[_numKernels];
        _selections = new int[_numKernels];
    }

    EuclidianKernels (const uint numKernels, const uint dims,
    		const double act, double ** kernels) :
        _numKernels(numKernels),
		_dims(dims),
        _tmp(nullptr),
		_kernels(kernels),
        _selections(nullptr),
        _boxes(nullptr)
    {
        if (_kernels == nullptr)
	        kernelgens::createRandomKernels(_dims, _numKernels, _kernels);

        _k = uint(ceil(_numKernels * act));

        if (_k == 0)
            _k = 1;

        _tmp = new double[dims];
    	_boxes = new BOX[_numKernels];
        _selections = new int[_numKernels];
    }

    virtual
    ~EuclidianKernels ()
    {
        for (uint i=0; i!=_numKernels; ++i)
            delete [] _kernels[i];

        delete [] _selections;
        delete [] _kernels;
        delete [] _boxes;
        delete [] _tmp;
    }

    const int *
	select(const double * const pattern)
    {
        for (uint i=0; i!=_dims; ++i)
            _tmp[i] = pattern[i];

        for (uint i=0; i!=_numKernels; ++i)
        {
            _boxes[i].w = -math::sdistance(_tmp, _kernels[i], _dims);
            _boxes[i].id = i;
        }

        wup::halfqsort(_boxes, 0, _numKernels-1, _numKernels-1);

        for (uint i=0; i!=_numKernels; ++i)
            _selections[i] = _boxes[i].id;

        return _selections;
    }

    uint
    k() const
    {
        return _k;
    }

    uint
    dims() const
    {
        return _dims;
    }

    uint
    numKernels() const
    {
        return _numKernels;
    }

    void
    exportTo(wup::IntWriter &writer)
    {
    	writer.put(-1);
        kernelgens::exportKernels(writer, _dims, _numKernels, _kernels);
        writer.putUInt32(_k);
        writer.put(-1);
    }

    bool
    operator !=(EuclidianKernels const& other) const
    {
    	return !(*this == other);
    }

    bool
    operator ==(EuclidianKernels const& other) const
    {
        if (_numKernels != other._numKernels)
            return false;

        if (_dims != other._dims)
            return false;

        if (_k != other._k)
            return false;

        for (uint i=0; i!=_numKernels; ++i)
        {
 			auto v1 = _kernels[i];
 			auto v2 = other._kernels[i];

            for (uint j=0; j!=_dims; ++j)
            {
                if (v1[j] != v2[j])
                    return false;
			}
 		}

 		return true;
 	}

private:

    uint _numKernels;

    uint _dims;

    double * _tmp;

    double ** _kernels;

    int * _selections;

    uint _k;

    BOX *_boxes;

};

template <typename KernelSpace=EuclidianKernels>
class KernelCanvas {
public:

    KernelCanvas(IntReader & reader) :
            _term_bits(reader.getUInt32()),
            _kernelSpace(reader),
            _outputFreq(new int[_kernelSpace.numKernels()]),
            _outputBits(new int[_kernelSpace.numKernels() * _term_bits])
    {
        if (reader.get() != -1)
            throw WUPException("Could not import kernelcanvas");
    }

    KernelCanvas(const uint inputs, const uint numKernels, const double act,
            const uint term_bits, double ** kernels=nullptr) :
        _term_bits(term_bits),
        _kernelSpace(numKernels, inputs, act, kernels),
        _outputFreq(new int[_kernelSpace.numKernels()]),
        _outputBits(new int[_kernelSpace.numKernels() * term_bits])
    {

    }

    ~KernelCanvas()
    {
        delete [] _outputFreq;
        delete [] _outputBits;
    }

    void
    clear()
    {
        for (uint i=0; i!=_kernelSpace.numKernels(); ++i)
            _outputFreq[i] = 0;
    }

    void read(const double * pattern)
    {
        const int * const ids = _kernelSpace.select( pattern );
        for (uint i=0; i!=_kernelSpace.k(); ++i)
            _outputFreq[ids[i]] = 1;
    }

    int * binary_output()
    {
        const int len = _kernelSpace.numKernels();
        int * current = _outputBits;

        for (uint i=0; i!=_term_bits; ++i)
        {
            std::copy(_outputFreq, _outputFreq + len, current);
            current += len;
        }

        return _outputBits;
    }

    int * real_output()
    {
        return _outputFreq;
    }

    uint binary_output_size() const
    {
        return _kernelSpace.numKernels() * _term_bits;
    }

    int real_output_size() const
    {
        return _kernelSpace.numKernels();
    }

    void exportTo(wup::IntWriter & writer)
    {
        writer.put(_term_bits);
        _kernelSpace.exportTo(writer);
        writer.put(-1);
    }

    KernelSpace &
    kernelSpace()
    {
        return _kernelSpace;
    }

    bool operator ==(wup::KernelCanvas<KernelSpace> const& other) const
    {
        if (_term_bits != other._term_bits)
            return false;

        if (_kernelSpace != other._kernelSpace)
            return false;

 		return true;
 	}

private:

    uint _term_bits;

    KernelSpace _kernelSpace;

    int * _outputFreq;

    int * _outputBits;

};

} /* wup */

#endif /* INCLUDE_WUP_MODELS_KERNELCANVAS_HPP_ */
