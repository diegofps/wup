#ifndef __WUP__ADDRESSDECODER
#define __WUP__ADDRESSDECODER

#include <wup/common.hpp>
#include <iostream>
#include <cstring>
#include <cmath>

namespace wup {

using namespace std;

class AddressDecoder {

public:

	AddressDecoder(istream &file_in)
	{
		file_in >> _inputs;
		file_in >> _bits;
		file_in >> _numRams;
		
		_shuffling = new int[_inputs];
		_output = new long long[_numRams];
		
		for (int i=0;i<_inputs;++i)
			file_in >> _shuffling[i];
		
		for (int i=0;i<_numRams;++i)
			file_in >> _output[i];
	}
	
    AddressDecoder(const int inputs, const int bits) : _inputs(inputs),
            _bits(bits)
    {
        _numRams = (int) ceil(float(inputs) / float(bits));
        _shuffling = randperm(inputs);
        _output = new long long[_numRams];
    }

    AddressDecoder(const AddressDecoder &other) : _inputs(other._inputs),
            _bits(other._bits)
    {
        warn("AddressDecoder copy!");

        _numRams = other._numRams;
        _shuffling = new int[_inputs];
        _output = new long long[_numRams];

        memcpy(_shuffling, other._shuffling, sizeof(int) * _inputs);
        memcpy(_output, other._output, sizeof(int) * _numRams);
    }

    ~AddressDecoder()
    {
        delete [] _shuffling;
        delete [] _output;
    }

    const long long * const read(const Pattern & pattern) const
    {
        _validate_pattern(pattern);
    	
        long long address;
        int k = 0;

        for (int r=0;r<_numRams;++r) {
            address = 0;

            const int top = wup::min(_inputs, k+_bits);
            for(;k<top;++k) address = (address << 1) + (pattern[_shuffling[k]]);

            _output[r] = address;
        }

        return _output;
    }

    const long long * const read(const int *pattern) const
    {	
    	return read(IntArray(pattern));
    }

    int numRams() const
    {
        return _numRams;
    }

    int bits() const
    {
        return _bits;
    }
    
    int inputs() const
    {
        return _inputs;
    }
    
	void dumpTo(ostream &file_out)
	{
		file_out << " " << _inputs;
		file_out << " " << _bits;
		file_out << " " << _numRams;
		
		for (int i=0;i<_inputs;++i)
			file_out << " " << _shuffling[i];
		
		for (int i=0;i<_numRams;++i)
			file_out << " " << _output[i];
		
		file_out << endl;
	}

	bool operator != (const AddressDecoder &other) const
	{
		return !((*this) == other);
	}
	
	bool operator == (const AddressDecoder &other) const
	{
		if (_inputs != other._inputs) return false;
		if (_numRams != other._numRams) return false;
		if (_bits != other._bits) return false;
		
		for (int i=0;i<_inputs;++i)
			if (_shuffling[i] != other._shuffling[i])
				return false;
		
		for (int i=0;i<_numRams;++i)
			if (_output[i] != other._output[i])
				return false;
		
		return true;
	}

private:
	
	void _validate_pattern(const IntArray & pattern) const
	{
		for (int i=0;i<_inputs;++i)
			if (pattern[i] != 0 && pattern[i] != 1)
				cout << "Warning: Strange bit " << pattern[i] 
					 << " at position " <<  i << endl;
	}
	
private:

    int _inputs;

    int _numRams;

    int _bits;

    int * _shuffling;

    long long * _output;

};

} /* wup */

#endif /* __WUP__ADDRESSDECODER */

