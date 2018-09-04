#ifndef __WUP__NODE__NODE_HPP
#define __WUP__NODE__NODE_HPP

#include <vector>
#include <cstdio>
#include <typeinfo>

#include <wup/common/dataset.hpp>
#include <wup/common/sbio.hpp>

namespace wup {

namespace node {

class Node {
private:

    wup::node::Node * _parent;

    double * _outputBuffer;

    Feature _output;

    ref_vector<Node> _children;

    ref_vector<Node> _emitters;

    int _patternLength;

    int * _patternOutput;

    bool _actsAsPattern;

public:

    Node(Node * const parent) :
        Node(parent, parent->output().size())
    {
//        LOGE("12");
    }

    Node(Node * const parent, const int outputSize) :
        _parent(parent),
        _outputBuffer(outputSize==0?NULL:new double[outputSize]),
        _output(_outputBuffer, outputSize),
        _patternLength(0),
        _patternOutput(NULL),
        _actsAsPattern(false)
    {
        //LOGE("11");
        if (parent != NULL)
            parent->addChild(this);
    }

    Node(Node * const parent, ireader & reader) :
        _parent(parent),
        _outputBuffer(NULL),
        _output(NULL, 0),
        _patternLength(0),
        _patternOutput(NULL),
        _actsAsPattern(false)
    {
        //LOGE("10");
        if (reader.get() != -1)
            throw new WUPException("Invalid file");

        int outputSize = reader.get();
        _actsAsPattern = reader.get();

        _outputBuffer = outputSize == 0 ? NULL : new double[outputSize];
        _output.remap(_outputBuffer, outputSize);

        if (reader.get() != -1)
            throw new WUPException("Invalid file");
    }

    void exportTo(iwriter & writer)
    {
        writer.put(-1);
        writer.put(_output.size());
        writer.put(_actsAsPattern);
        writer.put(-1);
        onExport(writer);
    }

    virtual
    ~Node()
    {
        if (_patternOutput != NULL)
            delete [] _patternOutput;

        if (_outputBuffer != NULL)
            delete [] _outputBuffer;

        for (auto &child : _children)
            delete &child;
    }

    virtual void
    onExport(iwriter & writer)
    {
        //LOGE("generic onExport");
    }

    Node *
    parent()
    {
        return _parent;
    }

    Node *
    root()
    {
        Node * result = this;
        while(result->_parent != NULL)
            result = result->_parent;
        return result;
    }

    Feature &
    output()
    {
        return _output;
    }

    const Feature &
    output() const
    {
        return _output;
    }

    void
    addChild(wup::node::Node * node)
    {
        _children.push_back(node);
        if (node->_actsAsPattern)
            node->actAsPattern();
    }
    
    const std::vector<Node*> &
    children()
    {
        return _children;
    }

    Node *
    lastDescendant()
    {
        if (_children.size() == 0)
            return this;
        else
            return _children[_children.size()-1].lastDescendant();
    }

    void
    clear()
    {
        onClear();
        for (auto &node : _children)
            node.clear();
    }

    void
    start()
    {
        onStart();
        for (auto &node : _children)
            node.start();
    }

    void
    digest(const Feature & feature)
    {
        //LOGE("Calling onDigest for an object of name: %s", typeid(*this).name());
        this->onDigest(feature);
    }

    void
    finish()
    {
        onFinish();
        for (Node &node : _children)
            node.finish();
    }

    void
    publish(const Feature & feature)
    {
        //LOGE("Publishing feature");
        for (Node & node : _children)
            node.digest(feature);
    }

    virtual void
    onClear()
    {

    }

    virtual void
    onStart()
    {

    }

    virtual void
    onDigest(const Feature & input)
    {
        //LOGE("Running default onDigest");
        publish(input);
    }

    virtual void
    onFinish()
    {

    }

    virtual void
    toPattern(int *)
    {
        throw new WUPException("This class may not be used as a pattern member");
    }
    
    virtual int
    patternSize()
    {
        return _patternLength;
    }

    void
    actAsPattern()
    {
        _actsAsPattern = true;
        root()->addPatternMember(*this);
    }

    void
    addPatternMember(Node & node)
    {
        _emitters.push_back(&node);
        _patternLength += node.patternSize();

        if (_patternOutput != NULL) {
            delete [] _patternOutput;
            _patternOutput = NULL;
        }
    }

    int *
    encode(const Sample &sample)
    {
        clear();
        start();
        for (auto &feature : sample) {
            const std::vector<Node*> &cs = children();
            for (auto &node : cs)
                node->digest(feature);
        }
        finish();

        return pattern();
    }

    int *
    pattern()
    {
        if (_patternOutput == NULL)
            _patternOutput = new int[_patternLength];

        int current = 0;
        for (auto & node : _emitters) {
            node.toPattern(&_patternOutput[current]);
            current += node.patternSize();
        }

        return _patternOutput;
    }

//    virtual void exportTo(wup::writer<double> &writer) {
//        for (auto child : _children) {
//            child->exportTo(writer);
//        }
//    }

};

} /* node */

} /* wup */

#endif
