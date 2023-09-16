#ifndef __WUP__NODE__EXPORT_HPP
#define __WUP__NODE__EXPORT_HPP

#include <wup/nodes/node.hpp>
#include <wup/common/bundle.hpp>
#include <wup/common/dataset.hpp>

namespace wup {

namespace node {

class Export : public Node {
public:
    
    Export(Node * const parent, const char * const filename) :
        Node(parent),
        _bundle(parent->output().size()),
        _filename(filename)
    {

    }

    Export(Node * const parent, IntReader & reader) :
        Node(parent, reader),
        _filename(reader.getString())
    {

    }

    virtual
    void onExport(IntWriter & writer)
    {
        writer.putString(_filename);
    }

    ~Export()
    {

    }

    void filename(const char * filename)
    {
        _filename = filename;
    }

    void filename(const std::string & filename)
    {
        _filename = filename;
    }

    virtual
    void onStart(const int & /*sampleId*/)
    {

    }

    virtual void
    onDigest(const Feature & input)
    {
        _bundle.push_many(input.data(), input.size());
    }

    virtual void onFinish()
    {
        std::ofstream file_out( _filename.c_str() );
        for (uint i=0; i<_bundle.rows(); ++i) {
            file_out << _bundle(i, uint(0));
            for (uint j=1; j<_bundle.cols(); ++j)
                file_out << "," << _bundle(i,j);
            file_out << std::endl;
        }

        _bundle.clear();
    }

private:
    Bundle<double> _bundle;

    std::string _filename;
};

} /* node */

} /* wup */

#endif
