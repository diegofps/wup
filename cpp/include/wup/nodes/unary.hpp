/*
 * unary.hpp
 *
 *  Created on: Feb 17, 2016
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_UNARY_HPP_
#define INCLUDE_WUP_NODES_UNARY_HPP_

namespace wup {

namespace node {

class Unary : public Node {
public:

	Unary(Node * const parent, const int termBits) :
		Node(parent, parent->outputLength() * termBits),
		_termBits(termBits)
	{ }

	virtual ~Unary()
	{
	}

    virtual void onDigest(const Feature & input)
    {
    	double * const data = feature().data();
    	for (int i=0;i<input.size();++i)
			termometer(&data[i*_termBits], input[i], _termBits);
    	yield(feature());
    }

    virtual void binaryOutput(int * dst)
    {
		Feature &f = feature();
    	for (int i=0;i<f.size();++i)
    		dst[i] = (int) round(f[i]);
    }

    virtual int binaryOutputLength()
    { return feature().size(); }

private:

    template <typename DST> void
	termometer(DST * const dst, const double value, const int termBits) {
    	const int threshold = min((value + 1.0) / 2.0 * termBits, termBits);
    	int i=0;
    	while (i<threshold) dst[i++] = 0;
    	while (i<termBits)  dst[i++] = 1;
    }

private:
    int _termBits;
};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_UNARY_HPP_ */
