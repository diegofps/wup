/*
 * delta.hpp
 *
 *  Created on: Feb 26, 2016
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_DELTA_HPP_
#define INCLUDE_WUP_NODES_DELTA_HPP_

#include <wup/nodes/node.hpp>

namespace wup {

namespace node {

class Delta : public Node {
public:

    Delta(Node * const parent) :
        Node(parent, parent->output().size()),
        _mem(new double[parent->output().size()]())
    {

    }

    ~Delta()
    {
        delete [] _mem;
    }

    virtual
    void onStart(const int & sampleId)
    {

    }

    virtual void
    digest(const Feature & input)
    {
        Feature & out = output();
        for (uint i=0;i<out.size();++i) {
            out[i] = input[i] - _mem[i];
            _mem[i] = input[i];
        }
        publish(out);
    }

    virtual void onFinish()
    {

    }

private:

    double * _mem;

};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_DELTA_HPP_ */
