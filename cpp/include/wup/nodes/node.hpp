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

    uint _patternLength;

    uint _realPatternLength;

    int * _patternOutput;

    double * _realPatternOutput;

    bool _actsAsPattern;

public:

    Node(Node * const parent) :
        Node(parent, parent->output().size())
    {

    }

    Node(Node * const parent, const uint outputSize) :
        _parent(parent),
        _outputBuffer(outputSize==0 ? nullptr : new double[outputSize]),
        _output(_outputBuffer, outputSize),
        _patternLength(0),
        _realPatternLength(0),
        _patternOutput(nullptr),
        _realPatternOutput(nullptr),
        _actsAsPattern(false)
    {
        if (parent != nullptr)
            parent->addChild(this);
    }

    Node(Node * const parent, ireader & reader) :
        _parent(parent),
        _outputBuffer(nullptr),
        _output(nullptr, 0),
        _patternLength(0),
        _realPatternLength(0),
        _patternOutput(nullptr),
        _realPatternOutput(nullptr),
        _actsAsPattern(false)
    {
        if (reader.get() != -1)
            throw new WUPException("Invalid file");

        uint outputSize = reader.getUnsignedInt();
        _actsAsPattern = reader.get();

        _outputBuffer = outputSize == 0 ? nullptr : new double[outputSize];
        _output.remap(_outputBuffer, outputSize);

        if (reader.get() != -1)
            throw new WUPException("Invalid file");
    }

    void exportTo(iwriter & writer)
    {
        writer.put(-1);
        writer.putUnsignedInt(_output.size());
        writer.put(_actsAsPattern);
        writer.put(-1);
        onExport(writer);
    }

    virtual
    ~Node()
    {
        if (_patternOutput != nullptr)
            delete [] _patternOutput;

        if (_realPatternOutput != nullptr)
            delete [] _realPatternOutput;

        if (_outputBuffer != nullptr)
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
        while(result->_parent != nullptr)
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
            return _children[_children.size() - 1].lastDescendant();
    }

    void
    clear()
    {
        onClear();
        for (auto &node : _children)
            node.clear();
    }

    void
    start(const int & sampleId)
    {
        onStart(sampleId);
        for (auto &node : _children)
            node.start(sampleId);
    }

    void
    digest(const Feature & feature)
    {
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
        for (Node & node : _children)
            node.digest(feature);
    }

    virtual void
    onClear()
    {

    }

    virtual void
    onStart(const int & sampleId)
    {

    }

    virtual void
    onDigest(const Feature & input)
    {
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

    virtual uint
    patternSize()
    {
        return _patternLength;
    }

    virtual uint
    realPatternSize()
    {
        return _realPatternLength;
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
        _realPatternLength += node.output().size();

        if (_patternOutput != nullptr)
        {
            delete [] _patternOutput;
            _patternOutput = nullptr;
        }

        if (_realPatternOutput != nullptr)
        {
            delete [] _realPatternOutput;
            _realPatternOutput = nullptr;
        }
    }

    int *
    encode(const Sample &sample)
    {
        clear();
        start(sample.id());

        for (auto &feature : sample)
        {
            const std::vector<Node*> &cs = children();
            for (auto &node : cs)
                node->digest(feature);
        }

        finish();
        return pattern();
    }

    double *
    encodeReal(const Sample &sample)
    {
        clear();
        start(sample.id());

        for (auto &feature : sample)
        {
            const std::vector<Node*> &cs = children();
            for (auto &node : cs)
                node->digest(feature);
        }

        finish();
        return realPattern();
    }

    int *
    pattern()
    {
        if (_patternOutput == nullptr)
            _patternOutput = new int[_patternLength];

        int current = 0;
        for (auto & node : _emitters) {
            node.toPattern(&_patternOutput[current]);
            current += node.patternSize();
        }

        return _patternOutput;
    }

    double *
    realPattern()
    {
        if (_realPatternOutput == nullptr)
            _realPatternOutput = new double[_realPatternLength];

        double * current = _realPatternOutput;
        for (auto & node : _emitters)
        {
            node.output().copyTo(current);
            current += node.output().size();
        }

        return _realPatternOutput;
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
