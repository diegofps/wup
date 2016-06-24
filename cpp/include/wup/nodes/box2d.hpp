/*
 * box2d.hpp
 *
 *  Created on: 02/04/2016
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_BOX2D_HPP_
#define INCLUDE_WUP_NODES_BOX2D_HPP_

namespace wup {

namespace node {

class Box2D : public Node {
public:

	Box2D(Node * const parent, const double left, const double right,
			const double top, const double bottom) :
		Node(parent, 2),
		_left(left),
		_right(right),
		_top(top),
		_bottom(bottom)
	{
		if (parent->outputLength() != 2)
			throw WUPException("Box2D requires an input pattern with length 2");
	}

    virtual void onStart()
    { }

    virtual void onFinish()
    { }

    virtual void onDigest(const Feature & input)
    {
		Feature & output = feature();
		output[0] = (input[0]-_left  )/(_right-_left) * 2.0 - 1.0;
		output[1] = (input[1]-_bottom)/(_top-_bottom) * 2.0 - 1.0;
		yield(output);
    }

private:
    double _left;
    double _right;
    double _top;
    double _bottom;

};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_BOX2D_HPP_ */
