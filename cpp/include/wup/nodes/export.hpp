#ifndef __WUP__NODE__EXPORT_HPP
#define __WUP__NODE__EXPORT_HPP

namespace wup {

namespace node {

class Export : public Node {
public:
    
    Export(Node * const parent, const char * const filename) :
        Node(parent),
		_bundle(parent->outputLength()),
		_filename(filename)
    { }
    
    ~Export()
    { }
    
    void onStart()
    { }

    void filename(const char * filename)
    { _filename = filename; }

    void filename(const std::string & filename)
    { _filename = filename; }

    void onFinish()
    {
    	//print("OnFinish");
    	std::ofstream file_out( _filename.c_str() );
        for ( int i=0;i<_bundle.numRows();++i ) {
            file_out << _bundle(i,0);
            for ( int j=1;j<_bundle.numCols();++j )
                file_out << "," << _bundle(i,j);
            file_out << std::endl;
        }
        
    	_bundle.clear();
    }

    virtual void onDigest(const Feature & input)
    {
    	_bundle.push_back(input.data(), input.size());
    }

private:
    Bundle<double> _bundle;
    std::string _filename;
};

} /* node */

} /* wup */

#endif
