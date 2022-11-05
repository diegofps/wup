#ifndef REPR_HPP
#define REPR_HPP

#include <string>

namespace wup {

class Repr
{
public:

    virtual std::string repr() const = 0;

};

inline std::ostream &
operator<<(std::ostream & o, Repr const & obj)
{
    o << obj.repr();
    return o;
}

}

#endif // REPR_HPP

