#ifndef __WUP__EXCEPTIONS
#define __WUP__EXCEPTIONS

#include <string>

namespace wup
{

class FatalException : public std::exception {
public:
    FatalException() : _msg("Unspecified") 
    { }
    
	FatalException(const std::string m) : _msg(m) 
	{ }
	
	~FatalException() throw() 
	{ }
	
	const char * what() const throw() 
	{ return _msg.c_str(); }
	
private:
	std::string _msg;
};

class ParsersException : public std::exception {
public:
	ParsersException(const char * const m) : _msg(m) 
	{ }
	
	ParsersException(const std::string &m) : _msg(m) 
	{ }
	
	~ParsersException() throw() 
	{ }
	
	const char * what() const throw() 
	{ return _msg.c_str(); }
	
private:
	std::string _msg;
};

class WUPException : public std::exception {
public:
    WUPException() : _msg("")
    { }
    
    WUPException(const std::string &msg) : _msg(msg)
    { }

    ~WUPException() throw()
    { }
	
    const char * what() const throw() 
    { return _msg.c_str(); }

private:
    std::string _msg;
    
};

}; /* wup */

#endif /* __WUP__EXCEPTIONS */

