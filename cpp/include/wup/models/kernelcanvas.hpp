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

class EuclideanKernels {
public:

    EuclideanKernels (wup::sbreader<double> &reader) :
            _numKernels(0),
            _dims(0),
            _tmp(NULL),
            _kernels(NULL),
            _selections(NULL),
            _boxes(NULL)
    {
    	if (reader.get() != -1)
    		throw new WUPException("Invalid file");

        kernelgens::importKernels(reader, _dims, _numKernels, _kernels);
        _k = (int) reader.get();

        if (reader.get() != -1)
			throw new WUPException("Invalid file");

        _tmp = new double[_dims];
        _boxes = new BOX[_numKernels];
        _selections = new int[_numKernels];
    }

    EuclideanKernels (const int numKernels, const int dims,
    		const double act, double ** kernels) :
        _numKernels(numKernels),
		_dims(dims),
		_tmp(NULL),
		_kernels(kernels),
		_selections(NULL),
		_boxes(NULL)
    {
    	if (_kernels == NULL)
	        kernelgens::createRandomKernels(_dims, _numKernels, _kernels);

    	_k = int(ceil(_numKernels * act));

        _tmp = new double[dims];
    	_boxes = new BOX[_numKernels];
        _selections = new int[_numKernels];
    }

    virtual
	~EuclideanKernels ()
    {
        for (int i=0;i<_numKernels;++i)
            delete [] _kernels[i];

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

        for (int i=0;i<_numKernels;++i)
        {
            _boxes[i].w = -wup::sdistance(_tmp, _kernels[i], _dims);
            _boxes[i].id = i;
        }

//        wup::halfqsort(_boxes, 0, _numKernels-1, _k-1);
        wup::halfqsort(_boxes, 0, _numKernels-1, _numKernels-1);

        for (int i=0;i< _numKernels; ++i)
            _selections[i] = _boxes[i].id;

        return _selections;
    }

    int
    k() const
    {
        return _k;
    }

    int
    numKernels() const
    {
        return _numKernels;
    }

    void
    exportTo(wup::sbwriter<double> &writer)
    {
    	writer.put(-1.0);
        kernelgens::exportKernels(writer, _dims, _numKernels, _kernels);
        writer.put((double)_k);
        writer.put(-1.0);
    }

    bool
    operator !=(EuclideanKernels const& other) const
    {
    	return !(*this == other);
    }

    bool
    operator ==(EuclideanKernels const& other) const
    {
        if (_numKernels != other._numKernels)
            return false;

        if (_dims != other._dims)
            return false;

        if (_k != other._k)
            return false;

        for (int i=0;i<_numKernels;++i)
        {
 			auto v1 = _kernels[i];
 			auto v2 = other._kernels[i];

            for (int j=0;j<_dims;++j)
            {
                if (v1[j] != v2[j])
                    return false;
			}
 		}

 		return true;
 	}

private:

    int _numKernels;

    int _dims;

    double * _tmp;

    double ** _kernels;

    int * _selections;

    int _k;

    BOX *_boxes;

};

template <typename KernelSpace=EuclideanKernels>
class KernelCanvas {
public:

    KernelCanvas(wup::sbreader<double> &reader) :
            _term_bits(reader.get()),
            _levels(reader.get()),
            _kernelSpace(reader),
            _outputFreq(new int[_kernelSpace.numKernels()]),
            _freshInk(new bool[_kernelSpace.numKernels()]),
            _outputBits(new int[_kernelSpace.numKernels() * _term_bits * _levels])
    {
        if (reader.get() != -1.0)
            throw new WUPException("Could not import kernelcanvas");
    }

    KernelCanvas(const int inputs, const int numKernels, const double act,
            const int term_bits, double ** kernels, const int levels=1) :
        _term_bits(term_bits),
        _levels(levels),
        _kernelSpace(numKernels, inputs, act, kernels),
        _outputFreq(new int[_kernelSpace.numKernels()]),
        _freshInk(new bool[_kernelSpace.numKernels()]),
        _outputBits(new int[_kernelSpace.numKernels() * term_bits * _levels])
    {

    }

    ~KernelCanvas()
    {
        delete [] _outputFreq;
        delete [] _outputBits;
        delete [] _freshInk;
    }

    void
    clear()
    {
        for (int i=0;i<_kernelSpace.numKernels();++i)
        {
            _outputFreq[i] = 0;
            _freshInk[i] = false;
        }
    }

    void
    clearInk()
    {
        for (int i=0;i<_kernelSpace.numKernels();++i)
            _freshInk[i] = false;
    }

    void read(const double * pattern)
    {
        int i = 0;
        const int * const ids = _kernelSpace.select( pattern );
        for (i=0; i<_kernelSpace.k(); ++i)
        {
            const int id = ids[i];
            if (!_freshInk[id])
            {
                ++_outputFreq[id];
                _freshInk[id] = true;
            }
        }

        for (i=3*_kernelSpace.k(); i<_kernelSpace.numKernels(); ++i)
            _freshInk[ids[i]] = false;
    }

    int * binary_output()
    {
        const int len = _kernelSpace.numKernels();

        for (int j=0;j<len;++j)
        {
            int i = 0;

            const int limit = min(_outputFreq[j], _levels);
            while (i < limit)
                _outputBits[j * _levels + i++] = 1;

            while (i < _levels)
                _outputBits[j * _levels + i++] = 0;
        }

        for (int i=1;i<_term_bits;++i)
            std::copy( _outputBits, _outputBits + (len * _levels), _outputBits + (i * len * _levels) );

        return _outputBits;
    }

    int * timestamp_output()
    {
        const int len = _kernelSpace.numKernels();

        for (int i=0;i<_term_bits;++i)
            std::copy(_outputFreq, &_outputFreq[len], &_outputBits[i * len]);

        return _outputBits;
    }

    double * real_output()
    {
        return _outputFreq;
    }

    int binary_output_size() const
    {
        return _kernelSpace.numKernels() * _term_bits * _levels;
    }

    int real_output_size() const
    {
        return _kernelSpace.numKernels();
    }

    int timestap_output_size()
    {
        return _kernelSpace.numKernels() * _term_bits;
    }

    void exportTo(wup::sbwriter<double> &writer)
    {
        writer.put((double)_term_bits);
        writer.put((double)_levels);
        _kernelSpace.exportTo(writer);
        writer.put(-1.0);
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

    int _term_bits;

    int _levels;

    KernelSpace _kernelSpace;

    int * _outputFreq;

    bool * _freshInk;

    int * _outputBits;

};

} /* wup */

#endif /* INCLUDE_WUP_MODELS_KERNELCANVAS_HPP_ */
