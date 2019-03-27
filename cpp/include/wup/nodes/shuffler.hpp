#ifndef SHUFFLER_HPP
#define SHUFFLER_HPP

#include <wup/nodes/node.hpp>
#include <wup/models/kernelcanvas.hpp>

namespace wup {

namespace node {

class Shuffler : public Node {

    uint _numIndexes;

    uint * _indexes;

public:

    Shuffler(Node * const parent, ireader & reader) :
            Node(parent, reader)
    {
        _numIndexes = reader.getUnsignedInt();
        _indexes = new uint[_numIndexes];

        for (uint i=0; i!=_numIndexes; ++i)
            _indexes[i] = reader.getUnsignedInt();
    }

    Shuffler(Node * const parent) :
        Node(parent)
    {
        _numIndexes = parent->output().size();
        _indexes = randperm(_numIndexes);
    }

    ~Shuffler()
    {
        delete [] _indexes;
    }

    virtual void
    onDigest(const Feature & input)
    {
        auto out = output();
        for (uint i=0;i!=_numIndexes;++i)
            out[i] = input[_indexes[i]];
        publish(out);
    }

    virtual void onExport(iwriter & writer)
    {
        writer.putUnsignedInt(_numIndexes);
        for (uint i=0; i!=_numIndexes; ++i)
            writer.putUnsignedInt(_indexes[i]);
    }

};

} /* node */

} /* wup */

#endif // SHUFFLER_HPP