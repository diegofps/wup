#ifndef __WUP__NODE__EXPORT_HPP
#define __WUP__NODE__EXPORT_HPP


namespace wup {

namespace node {

class Export : public Node {
public:
    
    Export(const Node * const parent, const char * const prefix) : 
        Node(parent, parent->outputLength()),
        _prefix(prefix),
        _counter(0)
    {
        
    }
    
    virtual void onDigest(Feature &src)
    {
        
    }
    
private:
    
    std::string _prefix;
    
    int _counter;
    
};

} /* node */

} /* wup */

#endif
