#ifndef __WUP__NODES_ADD_HPP
#define __WUP__NODES_ADD_HPP

#include <wup/nodes/node.hpp>

namespace wup {

namespace node {

class Add : public Node {
public:

    Add(Node * const parent) :
        Node(parent)
    {

    }

    Add(Node * const parent, IntReader & reader) :
        Node(parent, reader)
    {

    }

    ~Add()
    {

    }

    virtual
    void onStart(const int & /*sampleId*/)
    {
        output() = 0;
    }

    virtual 
    void onDigest(const Feature & input)
    {
        const double * const srcEnd = input.data() + input.size();
        const double  * src = input.data();
        double * dst = output().data();

        while (src != srcEnd)
        {
            *dst = *dst + *src;
            ++dst;
            ++src;
        }

        publish(output());
    }

};

} /* node */

} /* wup */

#endif /* __WUP__NODES_ADD_HPP */
