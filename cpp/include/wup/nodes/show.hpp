#ifndef __WUP__NODES_SHOW_HPP
#define __WUP__NODES_SHOW_HPP

namespace wup {

namespace node {

class Show : public Node {
public:

    Show(wup::node::Node * const parent,
			const char * const label="Unnamed",
			bool eachStep=false) :
        Node(parent),
		_bundle(parent->outputLength()),
		_counter(0),
		_eachStep(eachStep),
		_label(label)
    { }
    
    ~Show()
    { }
    
    void onStart()
    { _counter = 0; }

    void onFinish()
    {
    	if (!_eachStep) {
    		print(_label, ": ", _bundle);
    		_bundle.clear();
    	}
    }

    virtual void onDigest(const Feature & input)
    {
    	if (_eachStep) {
    		print(_label, ": ", input);
    	} else {
    		_bundle.push_back(input.data(), input.size());
    	}

    }

private:
    Bundle<double> _bundle;
    int _counter;
    bool _eachStep;
    const char * const _label;
};

} /* node */

} /* wup */

#endif /* __WUP__NODES_SHOW_HPP */
