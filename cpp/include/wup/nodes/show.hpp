#ifndef __WUP__NODES_SHOW_HPP
#define __WUP__NODES_SHOW_HPP

#include <wup/nodes/node.hpp>

namespace wup {

namespace node {

class Show : public Node {
private:

    Bundle<double> _bundle;

    int _counter;

    bool _eachStep;

    const std::string _label;

public:

    Show(Node * const parent,
            const char * const label="Unnamed",
            bool eachStep=false) :
        Node(parent),
        _bundle(parent->output().size()),
        _counter(0),
        _eachStep(eachStep),
        _label(label)
    {

    }

    Show(Node * const parent, sbreader<double> & reader) :
        Node(parent, reader),
        _bundle(reader.get()),
        _counter(reader.get()),
        _eachStep(reader.get()),
        _label(reader.getString())
    {

    }

    virtual
    void onExport(sbwriter<double> & writer)
    {
        writer.put(_bundle.numCols());
        writer.put(_counter);
        writer.put(_eachStep);
        writer.put(_label);
    }

    ~Show()
    {

    }
    
    virtual void
    clear()
    {
        _counter = 0;
    }

    virtual void
    onStart()
    {

    }

    virtual void
    onDigest(const Feature & input)
    {
        if (_eachStep)
        {
#ifdef __ANDROID_API__
            std::stringstream ss;
            ss << _label.c_str() << ": ";
            for (int j=0;j<input.size(); ++j)
                ss << input[j] << " ";
            LOGE("%s", ss.str().c_str());
#else
            print(_label, ": ", input);
#endif
        }
        else
        {
            _bundle.push_back(input.data(), input.size());
        }

        publish(input);
    }

    virtual void
    onFinish()
    {
//        LOGD("2");
        if (!_eachStep)
        {
#ifdef __ANDROID_API__
            const int numRows = _bundle.numRows();
            const int numCols = _bundle.numCols();
            std::stringstream ss;

            LOGD("%s", _label.c_str());
            for (int i=0;i<numRows; ++i) {
                ss.clear();
                for (int j=0;j<numCols; ++j)
                    ss << _bundle(i, j) << ", ";
                LOGD("%d: %s", i, ss.str().c_str());
            }
#else
            print(_label, ": ", _bundle);
#endif
            _bundle.clear();
        }
    }

};

} /* node */

} /* wup */

#endif /* __WUP__NODES_SHOW_HPP */
