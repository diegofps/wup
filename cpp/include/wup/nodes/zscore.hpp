/*
 * innernorm.hpp
 *
 *  Created on: 12/12/2015
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_ZSCORE_HPP_
#define INCLUDE_WUP_NODES_ZSCORE_HPP_

#include <vector>
#include <initializer_list>

#include <wup/nodes/node.hpp>

namespace wup {

namespace node {

class ZScore : public Node {
private:

    std::vector<double> _cache;

    double * _s1;

    double * _s2;

    seq<uint> _columns;

    bool _useHighestStd;

public:

    ZScore(Node * const parent, const bool useHighestStd=false) :
        Node(parent),
        _columns(parent->output().size()),
        _useHighestStd(useHighestStd)
    {
        _s1 = new double[_columns.size()];
        _s2 = new double[_columns.size()];

        for (uint i=0;i<_columns.size();++i)
            _columns[i] = i;
    }

    ZScore(Node * const parent, const seq<uint> &columns, const bool useHighestStd=false) :
        Node(parent),
        _columns(columns),
        _useHighestStd(useHighestStd)
    {
        _s1 = new double[columns.size()];
        _s2 = new double[columns.size()];
    }

    ZScore(Node * const parent, IntReader & reader) :
        Node(parent, reader),
        _columns(reader.get()),
        _useHighestStd(reader.getBool())
    {
        _s1 = new double[_columns.size()];
        _s2 = new double[_columns.size()];

        for (uint i=0; i<_columns.size(); ++i)
            _columns[i] = reader.get();
    }

    virtual
    void onExport(IntWriter & writer)
    {
        writer.putUnsignedInt(_columns.size());
        writer.putBool(_useHighestStd);

        for (uint i=0; i!=_columns.size(); ++i)
            writer.putUnsignedInt(_columns[i]);
    }

    virtual ~ZScore()
    {
        delete [] _s1;
        delete [] _s2;
    }

    virtual
    void onStart(const int & sampleId)
    {
        _cache.clear();
    }

    virtual void
    onDigest(const Feature & input)
    {
        for (uint i=0;i<input.size();++i)
            _cache.push_back(input[i]);
    }

    virtual void
    onFinish()
    {
        Feature & f = output();

        for (uint j=0; j!=_columns.size(); ++j)
            _s1[j] = _s2[j] = 0.0;

        for (ulong i=0; i<_cache.size(); i+=f.size())
        {
            for (uint j=0;j<_columns.size();++j)
            {
                const double v = _cache[i+_columns[j]];
                _s1[j] += v;
                _s2[j] += v*v;
            }
        }

        if (_useHighestStd) {
            double highest = -1.0;
            for (uint j=0; j<_columns.size(); ++j)
            {
                meanNstd(_cache.size()/f.size(),
                        _s1[j], _s2[j], _s1[j], _s2[j]);

                if (_s2[j] > highest)
                    highest = _s2[j];
            }

            for (ulong i=0; i < _cache.size(); i+= f.size())
            {
                for (uint j=0;j<f.size();++j)
                    f[j] = _cache[i+j];

                for (uint j=0;j<_columns.size();++j)
                    f[_columns[j]] = (f[_columns[j]] - _s1[j]) / highest;

                publish(f);
            }
        }
        else
        {
            for (uint j=0;j <_columns.size(); ++j)
                meanNstd(_cache.size()/f.size(),
                        _s1[j], _s2[j], _s1[j], _s2[j]);

            for (ulong i=0; i<_cache.size(); i+=f.size())
            {
                for (uint j=0;j<f.size();++j)
                    f[j] = _cache[i+j];

                for (uint j=0; j<_columns.size(); ++j)
                    f[_columns[j]] = (f[_columns[j]] - _s1[j]) / _s2[j];

                publish(f);
            }
        }

        _cache.clear();
    }

};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_ZSCORE_HPP_ */
