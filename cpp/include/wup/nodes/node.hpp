#ifndef __WUP__NODE__NODE_HPP
#define __WUP__NODE__NODE_HPP

#include <vector>

namespace wup {

namespace node {

class Node {
public:

    Node(Node * const parent) :
        Node(parent, parent->outputLength())
    { }
    
    Node(Node * const parent, const int size) :
        _parent(parent),
        _buffer(size==0?NULL:new double[size]),
        _feature(_buffer, size)
    { if (parent != NULL) parent->addChild(this); }
    
    virtual ~Node()
    { if (_buffer != NULL) delete [] _buffer; }
    
    double * output()
    { return _buffer; }
    
    int outputLength() const
    { return _feature.size(); }
    
    Node * parent()
    { return _parent; }
    
    Feature & feature()
    { return _feature; }
    
    void addChild(wup::node::Node * node)
    { _children.push_back(node); }
    
    std::vector<Node*> & children()
    { return _children; }
    
    void start()
    {
        onStart();
        for (Node * node : _children)
            node->start();
    }
    
    void finish()
    {
        onFinish();
        for (Node * node : _children)
            node->finish();
    }
    
    void
    digest(Feature &feature)
    { onDigest(feature); }
    
    void yield(const Feature & feature)
    {
        for (Node * node : _children)
            node->onDigest(feature);
    }
    
    
    virtual void onStart()
    { }
    
    virtual void onFinish()
    { }
    
    virtual void onDigest(const Feature & input)
    { yield(input); }
    
    
    virtual void binaryOutput(int * dst)
    { }
    
    virtual int binaryOutputLength()
    { return 0; }
    
private:
    
    wup::node::Node * _parent;
    
    double * _buffer;
    
    Feature _feature;
    
    std::vector<Node*> _children;
    
};

} /* node */

} /* wup */

#endif
