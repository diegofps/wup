#ifndef SHUFFLER_HPP
#define SHUFFLER_HPP

#include <wup/nodes/node.hpp>
#include <wup/models/kernelcanvas.hpp>
#include <wup/common/random.hpp>

namespace wup {

namespace node {

class Shuffler : public Node {

    uint _numIndexes;

    uint * _indexes;

    wup::random generator;

public:

    Shuffler(Node * const parent, IntReader & reader) :
            Node(parent, reader)
    {
        _numIndexes = reader.getUInt32();
        _indexes = new uint[_numIndexes];

        for (uint i=0; i!=_numIndexes; ++i)
            _indexes[i] = reader.getUInt32();
    }

    Shuffler(Node * const parent) :
        Node(parent)
    {
        _numIndexes = parent->output().size();
        _indexes = generator.randperm(_numIndexes);
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

    virtual void onExport(IntWriter & writer)
    {
        writer.putUInt32(_numIndexes);
        for (uint i=0; i!=_numIndexes; ++i)
            writer.putUInt32(_indexes[i]);
    }

};

} /* node */

} /* wup */

#endif // SHUFFLER_HPP
