#ifndef __WUP__NODES_SMOOTH4_HPP
#define __WUP__NODES_SMOOTH4_HPP

#include <wup/nodes/node.hpp>

namespace wup {

namespace node {

class Smooth4 : public Node {
private:

    double _minDistance;

    bool _firstFeature;

public:

    Smooth4(Node * const parent, const double minDistance) :
        Node(parent),
        _minDistance(sqrt(parent->output().size()) * minDistance),
        _firstFeature(true)
    {

    }

    Smooth4(Node * const parent, ireader & reader) :
        Node(parent, reader),
        _minDistance(reader.getDouble()),
        _firstFeature(reader.getBool())
    {

    }

    virtual
    void onExport(iwriter & writer)
    {
        writer.putDouble(_minDistance);
        writer.putBool(_firstFeature);
    }

    ~Smooth4()
    {

    }
    
    virtual void onStart()
    {
        _firstFeature = true;
    }

    virtual void onDigest(const Feature & input)
    {
        if (_firstFeature) {
            _firstFeature = false;
            output() = input;
            publish(input);
        }
        
        else if (sdistance(input, output()) > _minDistance) {
            output() = input;
            publish(input);
        }
    }

    virtual void onFinish()
    {

    }

};

} /* node */

} /* wup */

#endif /* __WUP__NODES_SMOOTH4_HPP */
