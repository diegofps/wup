/*
 * stdnorm.hpp
 *
 *  Created on: 11/12/2015
 *      Author: diegofps
 */

#ifndef __WUP__NODES__STDNORM_HPP
#define __WUP__NODES__STDNORM_HPP

#include <vector>

namespace wup {

namespace node {

class HighStdNorm : public Node {
public:

	HighStdNorm(wup::node::Node * const parent) :
		Node(parent)
	{
    	_s1 = new double[parent->outputLength()];
    	_s2 = new double[parent->outputLength()];
	}

	virtual ~HighStdNorm()
	{
		delete [] _s1;
		delete [] _s2;
	}

    virtual void onStart()
    { _data.clear(); }

    virtual void onFinish()
    {
		Feature & f = feature();
    	const int columns = f.size();

    	for (int j=0;j<columns;++j)
    		_s1[j] = _s2[j] = 0.0;

    	for (int i=0; i < _data.size(); i+= columns) {
    		for (int j=0;j<columns;++j) {
    			const double v = _data[i+j];
    			_s1[j] += v;
    			_s2[j] += v*v;
    		}
    	}

    	double highestStd = -1.0;
		for (int j=0;j<columns;++j) {
			meanNstd(_data.size()/columns,
					_s1[j], _s2[j], _s1[j], _s2[j]);

			if (_s2[j] > highestStd)
				highestStd = _s2[j];
		}

		for (int i=0; i < _data.size(); i+= columns) {
			for (int j=0;j<columns;++j)
				f[j] = (_data[i+j] - _s1[j]) / highestStd;
			yield(f);
		}

		_data.clear();
    }

    virtual void onDigest(const Feature & input)
    {
    	for (int i=0;i<input.size(); ++i)
    		_data.push_back(input[i]);
    }


private:

	std::vector<double> _data;

	double * _s1;

	double * _s2;

};

} /* node */

} /* wup */

#endif /* __WUP__NODES__STDNORM_HPP */
