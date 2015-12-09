#ifndef __WUP_EXCEPTIONS_HPP
#define __WUP_EXCEPTIONS_HPP

namespace wup {

class WUPException {
public:
    WUPException() : _msg("")
        { }
    
    WUPException(const char * const msg) : _msg(msg)
        { }
    
    const char * what() const
        { return _msg; }
        
private:
    const char * const _msg;
    
};

} /* wup */

#endif /* __WUP_EXCEPTIONS_HPP */

