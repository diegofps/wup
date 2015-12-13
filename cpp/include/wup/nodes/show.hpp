#ifndef __WUP__NODES_SHOW_HPP
#define __WUP__NODES_SHOW_HPP

namespace wup {

namespace node {

class Show : public Node {
public:
    Show(wup::node::Node * const parent) :
        Node(parent),
		_bundle(parent->outputLength()),
		_counter(0)
    { }
    
    ~Show()
    { }
    
    void onStart()
    {
    	print("OnStart");
    	_counter = 0;
    }

    void onFinish()
    {
    	print("OnFinish");
    	print(_bundle);
    	_bundle.clear();
    }

    virtual void onDigest(const Feature & input)
    {
    	print("onDigest ", _counter);
    	_bundle.push_back(input.data(), input.size());
    }

private:
    Bundle<double> _bundle;
    int _counter;
};

} /* node */

} /* wup */

#endif /* __WUP__NODES_SHOW_HPP */
