#ifndef __WUP__NODE__REPLICATE_HPP
#define __WUP__NODE__REPLICATE_HPP

#include <vector>

#include "node.hpp"

namespace wup {

namespace node {

class Replicate : public Node {
public:
    
    Replicate(Node * const parent, const int times) :
		Node(parent, parent->outputLength() * times),
		_times(times),
		_mem(new double[parent->outputLength()]),
		_current(0)
    {
    	if (times < 1)
    		throw new WUPException(cat("Can't replicate ", times, " times"));
    }

    Replicate(const Replicate & other) :
		Node(other),
		_times(other._times),
		_mem(new double[other.outputLength()]),
		_current(other._current)
    { memcpy(_mem, other._mem, sizeof(double) * other.outputLength()); }

    virtual ~Replicate()
    {
		delete [] _mem;
    }

    virtual void onStart()
    { feature() = 0.0; }

    virtual void onDigest(const Feature & input)
    {
    	Feature & output = feature();
    	const int length = input.size();

    	for (int k=_times-1;k>0;--k) {
    		const int current  = k * length;
    		const int previous = (k-1) * length;

    		memcpy(output.data() + current, output.data() + previous,
    				sizeof(double) * length);
    	}

    	memcpy(output.data(), input.data(),
    			sizeof(double) * length);

        yield(output);
    }

    virtual void binaryOutput(int * dst)
    {
    	for (int i=0;i<feature().size();++i)
    		dst[i] = feature()[i] >= 0.5 ? 1 : 0;
    }

    virtual int binaryOutputLength()
    { return feature().size(); }
    
private:

    int _times;
    
    double * _mem;

    int _current;

};

} /* node */

} /* wup */

#endif
