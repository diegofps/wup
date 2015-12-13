/*
 * innernorm.hpp
 *
 *  Created on: 12/12/2015
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_ZSCORE_HPP_
#define INCLUDE_WUP_NODES_ZSCORE_HPP_

#include <vector>

namespace wup {

namespace node {

class ZScore : public Node {
public:

	ZScore(Node * const parent, const bool useHighestStd=false) :
		Node(parent),
		_columns(),
		_useHighestStd(useHighestStd)
	{
		const int length = parent->outputLength();
		_s1 = new double[length];
		_s2 = new double[length];
		_columns.resize(length);

		for (int i=0;i<length;++i)
			_columns[i] = i;
	}

	ZScore(Node * const parent, const seq<int> & columns, const bool useHighestStd=false) :
		Node(parent),
		_columns(columns),
		_useHighestStd(useHighestStd)
	{
    	_s1 = new double[_columns.size()];
    	_s2 = new double[_columns.size()];
	}

	virtual ~ZScore()
	{
		delete [] _s1;
		delete [] _s2;
	}

    virtual void onStart()
    { _data.clear(); }

    virtual void onFinish()
    {
		Feature & f = feature();

    	for (int j=0;j<_columns.size();++j)
    		_s1[j] = _s2[j] = 0.0;

    	for (int i=0; i < _data.size(); i+= f.size()) {
    		for (int j=0;j<_columns.size();++j) {
    			const double v = _data[i+_columns[j]];
    			_s1[j] += v;
    			_s2[j] += v*v;
    		}
    	}

		if (_useHighestStd) {

			double highest = -1.0;
			for (int j=0;j<_columns.size();++j) {
				meanNstd(_data.size()/f.size(),
						_s1[j], _s2[j], _s1[j], _s2[j]);
				if (_s2[j] > highest)
					highest = _s2[j];
			}

			for (int i=0; i < _data.size(); i+= f.size()) {
				for (int j=0;j<f.size();++j)
					f[j] = _data[i+j];

				for (int j=0;j<_columns.size();++j)
					f[_columns[j]] = (f[_columns[j]] - _s1[j]) / highest;
				yield(f);
			}
		}

		else {

			for (int j=0;j<_columns.size();++j)
				meanNstd(_data.size()/f.size(),
						_s1[j], _s2[j], _s1[j], _s2[j]);

			for (int i=0; i < _data.size(); i+= f.size()) {
				for (int j=0;j<f.size();++j)
					f[j] = _data[i+j];

				for (int j=0;j<_columns.size();++j)
					f[_columns[j]] = (f[_columns[j]] - _s1[j]) / _s2[j];
				yield(f);
			}
		}

		_data.clear();

    }

    virtual void onDigest(const Feature & input)
    {
    	for (int i=0;i<input.size();++i)
    		_data.push_back(input[i]);
    }

private:
	std::vector<double> _data;
	double * _s1;
	double * _s2;
	seq<int> _columns;
	bool _useHighestStd;
};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_ZSCORE_HPP_ */
