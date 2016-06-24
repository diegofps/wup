#ifndef __WUP__NODE__ROOT_HPP
#define __WUP__NODE__ROOT_HPP

#include <vector>

namespace wup {

namespace node {

class Root : public Node {
public:
    
    Root(const int size) : 
		Node(NULL, size),
		_binaryOutputLength(0),
		_binaryOutput(NULL)
    {
    	if (size <= 0)
    		throw WUPException("Number of columns must be bigger or equal to one");
    }
    
    virtual ~Root()
    {
        if (_binaryOutput != NULL)
            delete [] _binaryOutput;
    }
    
    int *
    digest(const Sample &sample)
    {
        this->start();
        for (auto &feature : sample) {
            std::vector<Node*> &cs = children();
            for (auto &node : cs)
                node->digest(feature);
        }
        this->finish();
        
        return binaryOutput();
    }

    int *
    digest(const Feature &feature)
    {
		for (auto &node : children())
			node->digest(feature);
        return binaryOutput();
    }
    
    void
    addEmitter(wup::node::Node * node)
    {
        _terminals.push_back(node);
        _binaryOutputLength += node->binaryOutputLength();
        
        if (_binaryOutput != NULL) {
            delete [] _binaryOutput;
            _binaryOutput = NULL;
        }
    }


    virtual void binaryOutput(int * dst)
    {
    	memcpy(dst, binaryOutput(),
    			sizeof(double) * _binaryOutputLength);
    }

    virtual int binaryOutputLength()
    {
    	return _binaryOutputLength;
    }

    int * binaryOutput()
    {
        if (_binaryOutput == NULL)
            _binaryOutput = new int[_binaryOutputLength];
        
        int current = 0;
        for (auto & node : _terminals) {
            node->binaryOutput(&_binaryOutput[current]);
            current += node->binaryOutputLength();
        }
        
        return _binaryOutput;
    }

private:
    std::vector<Node*> _terminals;
    int _binaryOutputLength;
    int * _binaryOutput;
};

} /* node */

} /* wup */

#endif
