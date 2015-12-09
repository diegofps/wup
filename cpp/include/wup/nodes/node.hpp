#ifndef __WUP__NODE__NODE_HPP
#define __WUP__NODE__NODE_HPP

#include <vector>
#include <wup/feature.hpp>

namespace wup {

namespace node {

class Node {
public:

    Node(wup::node::Node * const parent, const int size) :
        _parent(parent),
        _feature(size)
    {
        if (parent != NULL)
            parent->addChild(this);
        
        if (size != 0)
            _buffer = new double[size];
    }
    
    virtual ~Node()
    {
        if (_buffer != NULL)
            delete [] _buffer;
    }
    
    double *
    output()
    {
        return _buffer;
    }
    
    int
    outputLength() const
    {
        return _size;
    }
    
    Node *
    parent()
    {
        return _parent;
    }
    
    void
    addChild(wup::node::Node * node)
    {
        _children.push_back(node);
    }
    
    void
    start()
    {
        onStart();
        for (auto & node : _children)
            node->start();
    }
    
    void
    finish()
    {
        onFinish();
        for (auto & node : _children)
            node->finish();
    }
    
    void
    yield(const double * buffer)
    {
        for (auto & node : _children)
            node->onDigest(buffer);
    }
    
    virtual void onStart() { }
    
    virtual void onFinish() { }
    
    virtual void onDigest(Feature &src)
    {
        throw wup::WUPException(
                "This node does not implement this functionality");
    }
    
    virtual void binaryOutput(int * dst)
    {
        throw wup::WUPException(
                "This node does not implement this functionality");
    }
    
    virtual int binaryOutputLength()
    {
        throw wup::WUPException(
                "This node does not implement this functionality");
    }
    
private:
    
    wup::node::Node * _parent;
    
    Feature _feature;
    
    std::vector<Node*> _children;
    
};

} /* node */

} /* wup */

#endif
