/*
 * appenddirection.hpp
 *
 *  Created on: 12/12/2015
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_DIRECTION_HPP_
#define INCLUDE_WUP_NODES_DIRECTION_HPP_

#include <wup/nodes/node.hpp>

namespace wup {

namespace node {

class Direction : public Node {
public:

    Direction(Node * const parent) :
        Node(parent, parent->output().size()+2),
        _lastX(0.0),
        _lastY(0.0),
        _isFirst(true)
    {

    }

    Direction(Node * const parent, IntReader & reader) :
        Node(parent, reader),
        _lastX(reader.getDouble()),
        _lastY(reader.getDouble()),
        _isFirst(reader.getBool())
    {

    }

    virtual
    void onExport(IntWriter & writer)
    {
        writer.putDouble(_lastX);
        writer.putDouble(_lastY);
        writer.putBool(_isFirst);
    }

    virtual
    void onStart(const int & /*sampleId*/)
    {
        _lastX = 0.0;
        _lastY = 0.0;
        _isFirst = true;
    }

    virtual void
    onDigest(const Feature & input)
    {
#ifndef WUP_UNSAFE
        if (input.size() < 2)
            throw WUPException("Feature is too short, need at least two columns");
#endif

        Feature & out = output();

        if (_isFirst)
        {
            memcpy(out.data(), input.data(),
                    sizeof(double) * input.size());

            _isFirst = false;
        }
        else
        {
            direction(out[0], out[1], input[0], input[1],
                    out[input.size()+0],
                    out[input.size()+1]);
            publish(out);

            memcpy(out.data(), input.data(),
                    sizeof(double) * input.size());
        }
    }

    virtual void
    onFinish()
    {
        publish(output());
    }

private:

    void direction(const double lx, const double ly,
            const double x, const double y,
            double &c, double &s) const
    {
        const double a = x - lx;
        const double o = y - ly;
        const double h = sqrt( a*a + o*o );

        if (h == 0.0)
        {
            warn("H is zero");
            c = 0.0;
            s = 0.0;
        }
        else
        {
            c = a / h;
            s = o / h;
        }
    }

private:
    double _lastX;
    double _lastY;
    bool _isFirst;
};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_DIRECTION_HPP_ */
