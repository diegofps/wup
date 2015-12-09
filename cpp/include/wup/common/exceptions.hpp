#ifndef __WUP__EXCEPTIONS
#define __WUP__EXCEPTIONS

#include <string>

namespace wup
{

using namespace std;

class FatalException : public exception {
public:
    FatalException() : _msg("Unspecified") { }
	FatalException(const string m) : _msg(m) { }
	~FatalException() throw() { }
	const char * what() const throw() { return _msg.c_str(); }
private:
	string _msg;
};

class ParsersException : public exception {
public:
	ParsersException(const char * const m) : _msg(m) { }
	ParsersException(const string &m) : _msg(m) { }
	~ParsersException() throw() { }
	const char * what() const throw() { return _msg.c_str(); }
private:
	string _msg;
};

}; /* wup */

#endif /* __WUP__EXCEPTIONS */

