#ifndef KERNELWISARD_HPP
#define KERNELWISARD_HPP

#include <unordered_map>
#include <wup/nodes/node.hpp>
#include <wup/models/kernelcanvas.hpp>

namespace wup {

namespace models {

class Ram {
public:

    uint * _mem;

    Ram() :
        _mem(nullptr)
    {

    }

    ~Ram()
    {
        if (_mem != nullptr)
            delete [] _mem;
    }

    void
    init(const uint capacity)
    {
        _mem = new uint[capacity]();
    }

    void
    learn(const int & address)
    {
        _mem[address] += 1;
    }

    uint
    predict(const int & address, const uint & bleach) const
    {
        return _mem[address] >= bleach ? 1 : 0;
    }

};

class KernelDiscriminator {
public:

    Ram * _rams;

    uint _numRams;

    uint _lastActivation;

    uint _capacity;

    KernelDiscriminator(const uint & numRams, const uint & capacity)
    {
        _numRams = numRams;
        _rams = new Ram[_numRams];
        _capacity = capacity;

        for (uint i=0;i!=_numRams;++i)
            _rams[i].init(capacity);
    }

    ~KernelDiscriminator()
    {
        delete [] _rams;
    }

    void
    learn(const uint ramId, const int * const addresses, const uint & numAddresses)
    {
        for (uint i=0; i!=numAddresses; ++i)
            _rams[ramId].learn(addresses[i]);
    }

    void
    clearActivation()
    {
        _lastActivation = 0;
    }

    void
    predict(const uint ramId, const int * const addresses, const uint & numAddresses, const uint & bleach)
    {
        for (uint i=0; i!=numAddresses; ++i)
            _lastActivation += _rams[ramId].predict(addresses[i], bleach);
    }

    uint
    activation() const
    {
        return _lastActivation;
    }

};

template <typename KernelSpace=EuclidianKernels>
class KernelWisard {
public:

    uint _numRams;

    KernelSpace _kernelSpace;

    std::unordered_map<uint, KernelDiscriminator*> _discriminators;

    int _k1;

    int _k2;

    int _w1;

    int _w2;

    KernelWisard(ireader &reader) :
        _kernelSpace(reader)
    {

    }

    KernelWisard(const uint & inputSize,
                 const uint & numDims,
                 const uint & numKernels,
                 const double & act) :
        _numRams(inputSize / numDims),
        _kernelSpace(numKernels, numDims, act, nullptr)
    {

    }

    ~KernelWisard()
    {
        for (auto pair : _discriminators)
            delete pair.second;
    }

    void
    learn(const double * const pattern, const uint & target)
    {
        KernelDiscriminator * current = getDiscriminator( target );
        const double * start = pattern;

        for (uint i=0;i!=_numRams;++i)
        {
            const int * const ids = _kernelSpace.select( start );
            current->learn(i, ids, _kernelSpace.k());
            start += _kernelSpace.dims();
        }

    }

    uint
    readBleaching(const double * const pattern)
    {
        for (uint b=0;;++b)
        {
            readBleaching(pattern, b);

            if (_w1 != _w2)
                return _k1;
            else if (_w1 == 0)
                return 0;
        }
    }

    void
    readBleaching(const double * const pattern, const uint & b)
    {
        clearActivations();
        calculateActivations(pattern, b);
        detectBestDiscriminators();
    }

    void
    clearActivations()
    {
        for (auto pair : _discriminators)
            pair.second->clearActivation();
    }

    void
    calculateActivations(const double * const pattern, const uint & b)
    {
        for (auto pair : _discriminators)
        {
            const double * start = pattern;

            for (uint i=0;i!=_numRams;++i)
            {
                const int * const ids = _kernelSpace.select( start );
                pair.second->predict(i, ids, _kernelSpace.k(), b);
                start += _kernelSpace.dims();
            }
        }
    }

    void
    detectBestDiscriminators()
    {
        _k1 = -1;
        _k2 = -1;
        _w1 = -1;
        _w2 = -1;

        for (auto d : _discriminators)
        {
            const int & currentAct = d.second->activation();

            if (currentAct > _w1)
            {
                _k2 = _k1;
                _w2 = _w1;

                _k1 = d.first;
                _w1 = currentAct;
            }

            else if (currentAct > _w2)
            {
                _k2 = d.first;
                _w2 = currentAct;
            }
        }
    }

    KernelDiscriminator * getDiscriminator(const uint & target)
    {
        auto it = _discriminators.find(target);
        if (it == _discriminators.end())
        {
            KernelDiscriminator * neue = new KernelDiscriminator(_numRams, _kernelSpace.numKernels());
            _discriminators[target] = neue;
            return neue;
        }

        return it->second;
    }

};

} /* models */

namespace node {

class KernelWisardNode : public Node {
public:

    wup::models::KernelWisard<EuclidianKernels> w;


    KernelWisardNode(Node * const parent, ireader & reader) :
            Node(parent, reader)
    {

    }

    KernelWisardNode(Node * const parent,
                     const uint & numDims,
                     const uint & numKernels,
                     const double & act) :
        Node(parent, 1),
        w(parent->output().size(), numDims, numKernels, act)
    {

    }

    virtual
    void onStart(const int & sampleId)
    {
        w.clearActivations();
    }

    virtual void
    onDigest(const Feature & input)
    {
        double * out = output().data();
        w.

        publish(output());
    }

    virtual void onFinish()
    {

    }

    virtual void onExport(iwriter & writer)
    {
        throw WUPException("Not implemented");
    }

};

} /* node */

} /* wup */

#endif // KERNELWISARD_HPP
