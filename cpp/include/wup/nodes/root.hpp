#ifndef __WUP__NODE__ROOT_HPP
#define __WUP__NODE__ROOT_HPP

#include <vector>
#include "node.hpp"

namespace wup {

namespace node {

class Root : public Node {
public:
    
    Root(const int size) : 
            Node(NULL, size), 
            _binaryBuffer_size(0), 
            _binaryOutput(NULL)
    { }
    
    ~Root()
    {
        if (_binaryOutput != NULL)
            delete [] _binaryOutput;
    }
    
    int * 
    digest(Sample &sample)
    {
        this->start();
        for (auto &feature : sample)
            for (auto &node : children())
                node->digest(feature.data());
        this->finish();
        
        return buildBinaryPattern();
    }
    
    void
    digest(Feature &feature)
    {
        for (auto &node : children())
            node->digest(feature.data());
    }
    
    void
    addEmitter(wup::node::Node * node)
    {
        _terminals.push_back(node);
        _binaryBufferSize += node->binaryOutputLength();
        
        if (_binaryOutput != NULL) {
            delete [] _binaryOutput;
            _binaryOutput = NULL;
        }
    }

    int *
    buildBinaryPattern()
    {
        if (_binaryOutputt == NULL)
            _binaryOutput = new int[_binaryBufferSize];
        
        int current = 0;
        for (auto & node : _terminals) {
            node->binaryOutput(&_binaryOutput[current]);
            current += node->binaryOutputLength();
        }
        
        return _binaryOutput;
    }

private:
    
    std::vector<Node*> _terminals;
    
    int _binaryBufferSize;
    
    int * _binaryOutput;
    
};

} /* node */

} /* wup */

#endif
