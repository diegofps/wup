#ifndef OLDKERNELCANVAS_HPP
#define OLDKERNELCANVAS_HPP

#include <wup/wup.hpp>

//typedef struct
//{
//    double w;
//    int id;
//} BOX;

namespace legacy
{

class EuclidianKernels
{

public:

    EuclidianKernels (const int dims, const int numKernels,
            const double act, double ** kernels) :
        _numKernels(numKernels), _dims(dims), _tmp(new double[dims]),
        _kernels(kernels), _selections(nullptr), _k(1),
        _boxes(nullptr)
    {
        if (_kernels == nullptr) {
            wup::warn("Generating random kernels");
            wup::kernelgens::createRandomKernels(_dims, _numKernels, _kernels);
        }

        _k = int(ceil(_numKernels * act));
        _boxes = new wup::BOX[_numKernels];
        _selections = new int[_k];
    }

    EuclidianKernels (const EuclidianKernels &other) :
        _numKernels(0),
        _dims(0),
        _tmp(nullptr),
        _kernels(nullptr),
        _selections(nullptr),
        _k(other._k),
        _boxes(new wup::BOX[_numKernels])
    {
        wup::error( "Unimplemented copy constructor in EuclidianKernels" );
    }

    virtual
    ~EuclidianKernels ()
    {
        for (uint i=0;i<_numKernels;++i)
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

        for (uint i=0;i<_numKernels;++i) {
            _boxes[i].w = -wup::sdistance(_tmp, _kernels[i], _dims);
            _boxes[i].id = i;
        }

        halfqsort(_boxes, 0, _numKernels-1, _k-1);

        for (int i=0;i< _k; ++i)
            _selections[i] = _boxes[i].id;

        return _selections;
    }

    const int *
    selectWithPrecision(const double * const pattern)
    {
        for (int i=0;i<_dims;++i)
            _tmp[i] = tanh(pattern[i]);

        for (uint i=0;i<_numKernels;++i) {
            _boxes[i].w = -wup::sdistance(_tmp, _kernels[i], _dims);
            _boxes[i].id = i;
        }

        topkqsort(_boxes, 0, _numKernels-1, _k-1);

        for (int i=0;i< _k; ++i)
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

private:

    uint _numKernels;

    const int _dims;

    double * _tmp;

    double ** _kernels;

    int * _selections;

    int _k;

    wup::BOX * _boxes;

};

template <typename KernelSpace>
class BaseKernelCanvas {
public:

    BaseKernelCanvas(const int inputs, const int numKernels, const double act,
            const int term_bits, double ** kernels, int _) :
        _term_bits(term_bits),
        _kernelSpace(inputs, numKernels, act, kernels),
        _outputFreq(new int[_kernelSpace.numKernels()]),
        _outputBits(new int[_kernelSpace.numKernels() * term_bits])
    {
        wup::print(inputs, " ", kernels, " ", act, " ", term_bits);
    }

    BaseKernelCanvas(const BaseKernelCanvas &other)
    {
        wup::error( "Unimplemented copy constructor in KernelCanvas" );
    }

    BaseKernelCanvas(wup::ireader & reader) :
        _kernelSpace(9, 9, 0, nullptr)
    {
        wup::error("Not implemented");
    }

    ~BaseKernelCanvas()
    {
        delete [] _outputFreq;
        delete [] _outputBits;
    }

    void clear()
    {
        for (int i=0;i<_kernelSpace.numKernels();++i)
            _outputFreq[i] = 0;
    }

    void
    clearInk()
    {

    }

    void read(const double * pattern)
    {
        const int * const ids = _kernelSpace.select( pattern );
        for (int i=0; i!=_kernelSpace.k(); ++i)
            _outputFreq[ids[i]] = 1;
    }

    int * binary_output()
    {
        int offset = 0;
        for (int i=0;i!=_kernelSpace.numKernels();++i) {
            term( _outputFreq[i], &_outputBits[offset], _term_bits );
            offset += _term_bits;
        }

        return _outputBits;
    }

    double * real_output()
    {
        return _outputFreq;
    }

    int binary_output_size() const
    {
        return _kernelSpace.numKernels() * _term_bits;
    }

    int real_outputs() const
    {
        return _kernelSpace.numKernels();
    }

    void
    exportTo(wup::iwriter & writer)
    {

    }

    bool operator==(const BaseKernelCanvas<KernelSpace> & other) const
    {
        return true;
    }

private:

    void term( const int value, int * const mem, const int bits ) const
    {
        for (int i=0; i!=bits; ++i)
            mem[i] = value;
    }

private:

    int _term_bits;

    KernelSpace _kernelSpace;

    int * _outputFreq;

    int * _outputBits;

};

typedef BaseKernelCanvas<EuclidianKernels> KernelCanvas;

}

#endif // OLDKERNELCANVAS_HPP
