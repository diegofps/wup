/*
 * delta.hpp
 *
 *  Created on: Feb 26, 2016
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_DELTA_HPP_
#define INCLUDE_WUP_NODES_DELTA_HPP_

namespace wup {

namespace node {

class Delta : public Node {
public:

	Delta(Node * const parent) :
		Node(parent, parent->outputLength()),
		_mem(new double[parent->outputLength()]())
	{ }

	~Delta()
	{ delete [] _mem; }

    virtual void onStart()
    { }

    virtual void onFinish()
    { }

    virtual void onDigest(const Feature & input)
    {
		Feature & output = feature();
		for (int i=0;i<output.size();++i) {
			output[i] = input[i] - _mem[i];
			_mem[i] = input[i];
		}
		yield(output);
    }

private:

    double * _mem;

};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_DELTA_HPP_ */
