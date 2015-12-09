#ifndef __WUP__NODE__REPLICATE_HPP
#define __WUP__NODE__REPLICATE_HPP

#include <vector>

#include "node.hpp"

namespace wup {

namespace node {

class Replicate : public Node {
public:
    
    Replicate(const Node * const parent, const int times) : 
            Node(parent, parent->outputLength() * times), 
            _times(times)
    { }
    
    virtual void onDigest(const double * src)
    {
        const int length = this->outputLength();
        double * dst = this->output();
        int current = 0;
        
        for (int k=0;k<_times;++k)
            for (int i=0;i<length;++i,++current)
                dst[current] = src[i];
        
        yield(dst);
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

    int _times;
    
};

} /* node */

} /* wup */

#endif
