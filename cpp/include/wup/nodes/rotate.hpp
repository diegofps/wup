/*
 * rotate.hpp
 *
 *  Created on: 12/12/2015
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_ROTATE_HPP_
#define INCLUDE_WUP_NODES_ROTATE_HPP_

#include <wup/nodes/node.hpp>

namespace wup {

namespace node {

class Rotate : public Node {
public:

    Rotate(Node * const parent) :
        Node(parent)
    {
        cosNsin(0.0, _c, _s);
    }

    Rotate(Node * const parent, ireader & reader) :
        Node(parent, reader)
    {

    }

    virtual
    void onExport(iwriter &)
    {

    }

    void angle(const double degrees)
    {
        cosNsin(degrees, _c, _s);
    }

    virtual
    void onStart(const int & sampleId)
    {

    }

    virtual void
    onDigest(const Feature & input)
    {
        Feature & o = output();
        o = input;

        const double newx = _c*input[0] - _s*input[1];
        const double newy = _s*input[0] + _c*input[1];
        o[0] = newx;
        o[1] = newy;

        publish(o);
    }

    virtual void onFinish()
    {

    }

private:

    void cosNsin(const double degrees, double & c, double & s)
    {
        const double radians = degrees / 180.0 * M_PI;
        c = std::cos( radians );
        s = std::sin( radians );
    }

private:
    double _c;
    double _s;
};

} /* node */

} /* wup */



#endif /* INCLUDE_WUP_NODES_ROTATE_HPP_ */
