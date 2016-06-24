#ifndef __WUP__NODES_STEPS_HPP
#define __WUP__NODES_STEPS_HPP

namespace wup {

namespace node {

class Steps : public Node {
public:
    Steps(wup::node::Node * const parent, const double minDistance) :
        Node(parent),
        _minDistance(minDistance),
        _firstFeature(true)
    { }
    
    ~Steps()
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

        while (sdistance(feature(), input) >= _minDistance) {
            const double rate = _minDistance / module(feature(), input);
            
			for (int i=0;i<input.size();++i)
				feature()[i] = feature()[i]
						 + (input[i] - feature()[i]) * rate;
			yield(feature());
        }

        if (sdistance(feature(), input) > 0.001) {
            feature() = input;
            yield(feature());
        }
    }
    
private:
    
    double module(const Feature & f1, const Feature & f2) {
    	double distance = 0.0;
    	for (int i=0;i<f1.size();++i) {
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

