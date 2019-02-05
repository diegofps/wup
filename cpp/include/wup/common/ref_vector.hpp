#ifndef REF_VECTOR_HPP
#define REF_VECTOR_HPP

#include <unordered_map>
#include <iostream>
#include <vector>

#include <wup/common/iterators.hpp>

namespace wup {

template<typename T>
class ref_vector : public std::vector<T*>
{
public:

    T & operator[](const uint i)
    { return *std::vector<T*>::operator[](i); }

    const T & operator[](const uint i) const
    { return *std::vector<T*>::operator[](i); }

    //void push_back(T & value)
    //{ std::vector<T*>::push_back(&value); }

    ref_iterator<T> begin()
    { return ref_iterator<T>(this->data()); }

    ref_iterator<T> end()
    { return ref_iterator<T>(this->data() + this->size()); }

    const_ref_iterator<T> begin() const
    { return const_ref_iterator<T>(this->data()); }

    const_ref_iterator<T> end() const
    { return const_ref_iterator<T>(this->data() + this->size()); }

    template <typename Func>
    std::unordered_map<std::result_of<Func>, T*> toUnorderedMap(Func &func) const
    {
        std::unordered_map<std::result_of<Func>, T*> result;
        for (auto &t : *this)
            result[func(t)] = &t;
        return result;
    }

    template <typename Func>
    std::unordered_map<std::result_of<Func>, T*> toUnorderedMap(Func &func)
    { return const_cast<const ref_vector<T>>(this).toUnorderedMap(func); }
};

template <typename T>
std::ostream & operator<<(std::ostream & o, const ref_vector<T> & rv)
{
    if (rv.size() == 0)
        return o << "ref_vector: []";

    o << "ref_vector: [" << rv[0];
    for (uint i=1; i!=rv.size(); ++i)
        o << ", " << rv[i];

    return o << "]";
}

}

#endif // REF_VECTOR_HPP
