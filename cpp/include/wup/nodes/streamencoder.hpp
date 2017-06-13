#ifndef STREAMENCODER_H
#define STREAMENCODER_H

#include <wup/wup.hpp>
#include <typeinfo>
#include <string>
#include <map>

namespace wup {

namespace node {

template <typename NODE>
Node * nodeReader(Node * parent, sbreader<double> & reader)
{
    return new NODE(parent, reader);
}

class StreamEncoder
{
private:

    Node * _root;

    Node * _last;

    std::map<std::string, Node *(*)(Node * parent, wup::sbreader<double> & reader)> _nodeReader;

public:

    StreamEncoder(const int columns) :
        _root(new Node(NULL, columns)),
        _last(_root)
    {
        registerNodeReaders();
    }

    StreamEncoder(sbreader<double> & reader) :
            _root(NULL),
            _last(NULL)
    {
        registerNodeReaders();

        if (reader.get() != -1.0)
            throw new WUPException("Invalid file");

        if (reader.get() == 1.0)
        {
            //LOGE("Has root, loading");
            _root = importNode(NULL, reader);
            _last = _root->lastDescendant();
        }

        if (reader.get() != -1.0)
            throw new WUPException("Invalid file");
    }

    ~StreamEncoder()
    {
        delete _root;
    }

    StreamEncoder &
    exportTo(sbwriter<double> & writer)
    {
        writer.put(-1.0);

        if (_root == NULL) {
            //LOGE("Has no root :(");
            writer.put(0.0);
        }
        else {
            //LOGE("Has root, exporting!");
            writer.put(1.0);
            exportNode(writer, _root);
        }

        writer.put(-1.0);
        return *this;
    }

    template <typename NODE, typename ...Args>
    StreamEncoder &
    add(const Args&... args)
    {
        _last = new NODE(_last, args...);
        return * this;
    }

    StreamEncoder &
    actAsPattern()
    {
        _last->actAsPattern();
        return *this;
    }

    Node *
    root()
    {
        return _root;
    }

    Node *
    last()
    {
        return _last;
    }

    template <typename NODE>
    StreamEncoder & addNodeReader()
    {
        _nodeReader[typeid(NODE).name()] = &nodeReader<NODE>;
        return *this;
    }

    void
    clear()
    {
        _root->clear();
    }

    void
    start()
    {
        _root->start();
    }

    void
    digest(const Feature & feature)
    {
        _root->digest(feature);
    }

    void
    finish()
    {
        _root->finish();
    }

    int
    patternSize()
    {
        return _root->patternSize();
    }

    int *
    encode(const Sample &sample)
    {
        return _root->encode(sample);
    }

    int *
    pattern()
    {
        return _root->pattern();
    }

private:

    void
    exportNode(sbwriter<double> & writer, Node * node)
    {
        const char * name = typeid(*node).name();
        writer.put(name);

        node->exportTo(writer);

        writer.put(node->children().size());
        //LOGE("Exporting node with type %s with %d children", name, node->children().size());
        for (auto & child : node->children())
            exportNode(writer, child);
    }

    Node *
    importNode(Node * parent, sbreader<double> & reader)
    {
        const std::string nodeName = reader.getString();
        auto it = _nodeReader.find(nodeName);
        if (it == _nodeReader.end())
            throw new WUPException("Unknown node type");

        Node * newNode = it->second(parent, reader);

        const int numChildren = (int)reader.get();
        //LOGE("Importing node of type %s with %d children, loading", nodeName.c_str(), numChildren);
        for (int i=0;i<numChildren;++i)
            newNode->addChild(importNode(newNode, reader));

        return newNode;
    }

    void
    registerNodeReaders()
    {
        addNodeReader<node::Node>();
        addNodeReader<node::Show>();
        addNodeReader<node::Direction>();
        addNodeReader<node::ZScore>();
        addNodeReader<node::Replicate>();
        addNodeReader<node::KernelCanvas>();
        addNodeReader<node::Tanh>();
        addNodeReader<node::Rotate>();
        addNodeReader<node::Export>();
        addNodeReader<node::Unary>();
        addNodeReader<node::Box2D>();
        addNodeReader<node::Steps>();
        addNodeReader<node::Smooth4>();
    }

};

}

}

#endif // STREAMENCODER_H
