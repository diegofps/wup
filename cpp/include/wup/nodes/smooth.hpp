#ifndef __WUP__NODES_SMOOTH_HPP
#define __WUP__NODES_SMOOTH_HPP

namespace wup {

namespace node {

class Smooth : public Node {
public:
    Smooth(wup::node::Node * const parent, const double minDistance) :
        Node(parent),
        _minDistance(sqrt(parent->outputLength()) * minDistance),
        _firstFeature(true)
    { }
    
    ~Smooth()
    { }
    
    virtual void onStart()
    { _firstFeature = true; }
    
    virtual void onFinish()
    { }
    
    virtual void onDigest(const Feature & input)
    {
        if (_firstFeature) {
            _firstFeature = false;
            feature() = input;
            yield(input);
        }
        
        else if (sdistance(input, feature()) > _minDistance) {
            feature() = input;
            yield(input);
        }
    }
    
private:
    
    double _minDistance;
    
    bool _firstFeature;
    
};

} /* node */

} /* wup */

#endif /* __WUP__NODES_SMOOTH_HPP */
