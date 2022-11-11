#ifndef REPLICATE_HPP
#define REPLICATE_HPP

#include <wup/nodes/node.hpp>

namespace wup {

namespace node {

class Replicate : public Node {

    uint _times;

public:

    Replicate(Node * const parent, const uint times) :
        Node(parent, parent->output().size() * times),
        _times(times)
    {

    }

    Replicate(Node * const parent, IntReader & reader) :
            Node(parent, reader)
    {
        _times = reader.getUInt32();
    }

    virtual void
    onDigest(const Feature & input)
    {
        double * out = output().data();
        for (uint i=0; i!=_times; ++i)
        {
            memcpy(out , input.data(), input.size() * sizeof(double));
            out += input.size();
        }

        publish(output());
    }

    virtual void onExport(IntWriter & writer)
    {
        writer.putUInt32(_times);
    }

};

} /* node */

} /* wup */

#endif // REPLICATE_HPP
