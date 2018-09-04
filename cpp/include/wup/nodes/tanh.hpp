/*
 * tanh.hpp
 *
 *  Created on: 12/12/2015
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_TANH_HPP_
#define INCLUDE_WUP_NODES_TANH_HPP_

#include <initializer_list>
#include <wup/common/seq.hpp>
#include <wup/nodes/node.hpp>

namespace wup {

namespace node {

class Tanh : public Node {    
private:

    seq<uint> _columns;

public:

    Tanh(Node * const parent) : Node(parent),
        _columns(parent->output().size())
    {
        for (uint i=0;i<_columns.size();++i)
            _columns[i] = i;
    }

    Tanh(Node * const parent, const seq<uint> & columns) :
        Node(parent),
        _columns(columns)
    {

    }

    Tanh(Node * const parent, ireader & reader) :
        Node(parent, reader),
        _columns(reader.get())
    {
        for (uint i=0; i<_columns.size(); ++i)
            _columns[i] = reader.get();
    }

    virtual
    void onExport(iwriter & writer)
    {
        writer.put(_columns.size());
        for (uint i=0;i<_columns.size(); ++i)
            writer.put(_columns[i]);
    }

    virtual void onStart()
    {

    }

    virtual void onDigest(const Feature & input)
    {
        output() = input;
        for (uint i=0; i<_columns.size(); ++i)
            output()[_columns[i]] = tanh(input[_columns[i]]);
        publish(output());
    }

    virtual void onFinish()
    {

    }

};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_TANH_HPP_ */
