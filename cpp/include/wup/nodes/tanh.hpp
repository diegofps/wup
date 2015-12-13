/*
 * tanh.hpp
 *
 *  Created on: 12/12/2015
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_TANH_HPP_
#define INCLUDE_WUP_NODES_TANH_HPP_

namespace wup {

namespace node {

class Tanh : public Node {
public:

	Tanh(Node * const parent) :
		Node(parent),
		_columns()
	{
		const int length = parent->outputLength();
		_columns.resize(length);

		for (int i=0;i<length;++i)
			_columns[i] = i;
	}

	Tanh(Node * const parent, const seq<int> & columns) :
		Node(parent),
		_columns(columns)
	{ }

    virtual void onDigest(const Feature & input)
    {
    	feature() = input;
    	for (int i=0;i<_columns.size();++i)
    		feature()[_columns[i]] = tanh(input[_columns[i]]);
    	yield(feature());
    }

private:
	seq<int> _columns;
};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_TANH_HPP_ */
