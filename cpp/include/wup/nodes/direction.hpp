/*
 * appenddirection.hpp
 *
 *  Created on: 12/12/2015
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_DIRECTION_HPP_
#define INCLUDE_WUP_NODES_DIRECTION_HPP_

namespace wup {

namespace node {

class Direction : public Node {
public:

	Direction(Node * const parent) :
		Node(parent, parent->outputLength()+2),
		_lastX(0.0),
		_lastY(0.0),
		_isFirst(true)
	{ }

    virtual void onStart()
    {
    	_lastX = 0.0;
    	_lastY = 0.0;
    	_isFirst = true;
    }

    virtual void onFinish()
    {
    	yield(feature());
    }

    virtual void onDigest(const Feature & input)
    {
    	if (input.size() < 2)
    		throw new WUPException("Feature is too short, need at least two columns");

		Feature & output = feature();

    	if (_isFirst) {
    		memcpy(output.data(), input.data(),
					sizeof(double) * input.size());

    		_isFirst = false;
    	} else {
			direction(output[0], output[1], input[0], input[1],
					output[input.size()+0],
					output[input.size()+1]);
			yield(output);

	    	memcpy(output.data(), input.data(),
	    			sizeof(double) * input.size());
    	}
    }

private:

    void direction(const double lx, const double ly,
    		const double x, const double y,
			double &c, double &s) const
    {
    	const double a = x - lx;
    	const double o = y - ly;
    	const double h = sqrt( a*a + o*o );
    	c = h == 0.0 ? 0.0 : a / h;
    	s = h == 0.0 ? 0.0 : o / h;
    }

private:
    double _lastX;
    double _lastY;
    bool _isFirst;
};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_DIRECTION_HPP_ */
