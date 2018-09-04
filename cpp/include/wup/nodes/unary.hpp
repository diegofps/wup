/*
 * unary.hpp
 *
 *  Created on: Feb 17, 2016
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_NODES_UNARY_HPP_
#define INCLUDE_WUP_NODES_UNARY_HPP_

#include <wup/nodes/node.hpp>

namespace wup {

namespace node {

class Unary : public Node {
private:

    int _termBits;

    template <typename DST> void
    termometer(DST * const dst, const double value, const int termBits) {
        const int threshold = min((value + 1.0) / 2.0 * termBits, termBits);
        int i=0;
        while (i<threshold) dst[i++] = 0;
        while (i<termBits)  dst[i++] = 1;
    }

public:

    Unary(Node * const parent, const int termBits) :
        Node(parent, parent->output().size() * termBits),
        _termBits(termBits)
    {

    }

    Unary(Node * const parent, ireader & reader) :
        Node(parent, reader),
        _termBits(reader.get())
    {

    }

    virtual
    void onExport(iwriter & writer)
    {
        writer.put(_termBits);
    }

    virtual ~Unary()
    {

    }

    virtual void onStart()
    {

    }

    virtual void
    onDigest(const Feature & input)
    {
        double * const data = output().data();
        for (uint i=0; i<input.size(); ++i)
            termometer(&data[i*_termBits], input[i], _termBits);
        publish(output());
    }

    virtual void onFinish()
    {

    }

    virtual void toPattern(int * dst)
    {
        Feature &f = output();
        for (uint i=0; i<f.size(); ++i)
            dst[i] = (int) round(f[i]);
    }

    virtual int patternSize()
    {
        return output().size();
    }

};

} /* node */

} /* wup */

#endif /* INCLUDE_WUP_NODES_UNARY_HPP_ */
