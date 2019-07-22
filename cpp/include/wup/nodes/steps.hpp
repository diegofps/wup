#ifndef __WUP__NODES_STEPS_HPP
#define __WUP__NODES_STEPS_HPP

#include <wup/nodes/node.hpp>

namespace wup {

namespace node {

class Steps : public Node {
public:

    Steps(Node * const parent, const double minDistance) :
        Node(parent),
        _minDistance(minDistance),
        _firstFeature(true)
    {

    }

    Steps(Node * const parent, IntReader & reader) :
        Node(parent, reader),
        _minDistance(reader.getDouble()),
        _firstFeature(reader.getBool())
    {

    }

    virtual
    void onExport(IntWriter & writer)
    {
        writer.putDouble(_minDistance);
        writer.putBool(_firstFeature);
    }

    ~Steps()
    {

    }

    virtual
    void onStart(const int & /*sampleId*/)
    {
        _firstFeature = true;
    }

    virtual void
    onDigest(const Feature & input)
    {
        if (_firstFeature) {
            _firstFeature = false;
            output() = input;
            publish(input);
        }

        while (sdistance(output(), input) >= _minDistance) {
            const double rate = _minDistance / module(output(), input);
            
            for (uint i=0;i<input.size();++i)
                output()[i] = output()[i]
                         + (input[i] - output()[i]) * rate;
            publish(output());
        }

        if (sdistance(output(), input) > 0.001) {
            output() = input;
            publish(output());
        }
    }

    virtual void onFinish()
    {

    }

private:
    
    double module(const Feature & f1, const Feature & f2) {
        double distance = 0.0;
        for (uint i=0; i<f1.size(); ++i) {
            const double v = f2[i] - f1[i];
            distance += v*v;
        }
        return sqrt(distance);
    }

private:

    double _minDistance;

    bool _firstFeature;
    
};

} /* node */

} /* wup */

#endif /* __WUP__NODES_STEPS_HPP */

