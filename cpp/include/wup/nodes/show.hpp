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
    int _onlySample;
    bool _noPrint;
    int _currentSample;

public:

    Show(Node * const parent,
         const char * const label="Unnamed",
         bool eachStep=false,
         int onlySample=0) :
        Node(parent),
        _bundle(parent->output().size()),
        _counter(0),
        _eachStep(eachStep),
        _label(label),
        _onlySample(onlySample),
        _noPrint(true)
    {
        _bundle.clear();
    }

    Show(Node * const parent, IntReader & reader) :
        Node(parent, reader),
        _bundle(reader.getUInt32()),
        _counter(reader.get()),
        _eachStep(reader.get()),
        _label(reader.getString()),
        _onlySample(reader.get()),
        _noPrint(true)
    {

    }

    virtual
    void onExport(IntWriter & writer)
    {
        writer.putUInt32(_bundle.numCols());
        writer.put(_counter);
        writer.putBool(_eachStep);
        writer.putString(_label);
        writer.put(_onlySample);
    }

    virtual
    void onStart(const int & sampleId)
    {
        //print(sampleId);
        _noPrint = _onlySample != -1 && sampleId != _onlySample;
        _currentSample = sampleId;
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
    onDigest(const Feature & input)
    {
        if (_noPrint)
        {
            publish(input);
            return;
        }

        if (_eachStep)
        {
#ifdef __ANDROID_API__
            std::stringstream ss;
            ss << _label.c_str() << ": ";
            for (int j=0;j<input.size(); ++j)
                ss << input[j] << " ";
            LOGE("<Sample id=%d label=%s>%s</sample>", _onlySample, _label, ss.str().c_str());
#else
            print("<sample id=", _onlySample, " label=", _label ,">", input, "</sample>");
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
        if (_noPrint)
            return;

        if (_eachStep)
            return;

#ifdef __ANDROID_API__
        const int numRows = _bundle.numRows();
        const int numCols = _bundle.numCols();
        std::stringstream ss;

        LOGD("%s : %d", _label.c_str(), _currentSample);
        for (int i=0;i<numRows; ++i) {
            ss.clear();
            for (int j=0;j<numCols; ++j)
                ss << _bundle(i, j) << ", ";
            LOGD("%d: %s", i, ss.str().c_str());
        }
#else
        print(_label, " : ", _currentSample);
        print(_bundle);
#endif
        _bundle.clear();
    }

};

} /* node */

} /* wup */

#endif /* __WUP__NODES_SHOW_HPP */
