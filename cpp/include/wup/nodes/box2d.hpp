/*
 * box2d.hpp
 *
 *  Created on: 02/04/2016
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_BOX2D_HPP_
#define INCLUDE_WUP_NODES_BOX2D_HPP_

#include <wup/nodes/node.hpp>

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
        if (parent->output().size() != 2)
            throw WUPException("Box2D requires an input pattern with length 2");
    }

    Box2D(Node * const parent, IntReader & reader) :
        Node(parent, reader),
        _left(reader.getDouble()),
        _right(reader.getDouble()),
        _top(reader.getDouble()),
        _bottom(reader.getDouble())
    {

    }

    virtual
    void onExport(IntWriter & writer)
    {
        writer.putDouble(_left);
        writer.putDouble(_right);
        writer.putDouble(_top);
        writer.putDouble(_bottom);
    }

    virtual
    void onStart(const int & /*sampleId*/)
    {

    }

    virtual void
    onDigest(const Feature & input)
    {
        Feature & out = output();
        out[0] = (input[0]-_left  )/(_right-_left) * 2.0 - 1.0;
        out[1] = (input[1]-_bottom)/(_top-_bottom) * 2.0 - 1.0;
        publish(out);
    }

    virtual void onFinish()
    {

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
