#ifndef WISARD_HPP
#define WISARD_HPP

#include <unordered_map>
#include <vector>
#include <cmath>
#include <map>

#include <wup/common/exceptions.hpp>
#include <wup/common/io.hpp>
#include <wup/common/random.hpp>
#include <wup/common/math.hpp>
#include <wup/models/decoders/binarydecoder.hpp>
#include <wup/models/decoders/graydecoder.hpp>
#include <wup/models/decoders/basendecoder.hpp>
#include <wup/models/decoders/snakedecoder.hpp>
#include <wup/models/decoders/intdecoder.hpp>
#include <wup/models/pattern.hpp>

namespace wup
{

template <typename Decoder=BinaryDecoder, bool IgnoreZeroAddress=false>
class BaseWisard;

typedef BaseWisard<IntDecoder> IntWisard;
typedef BaseWisard<BinaryDecoder> Wisard;
typedef BaseWisard<GrayDecoder> GrayWisard;
typedef BaseWisard<BinaryDecoder, true> Wisardz;
typedef BaseWisard<GrayDecoder, true> GrayWisardz;

template <typename Decoder, bool IgnoreZeroAddress>
class BaseWisard 
{
public:
    
    random r;
    typedef std::map<int, int> MultiDiscriminator;
    typedef std::unordered_map<Decoder, MultiDiscriminator> Ram;

    BaseWisard(const int inputBits, const int ramBits) :
            BaseWisard(inputBits, ramBits, 2)
    { }
    
    BaseWisard(const int numInputBits, const int numRamBits, const int numClasses) :
            _confidence(1.0), 
            _rams(NULL),
            _maxBleaching(1), 
            _activations(NULL), 
            _activationsCapacity(numClasses),
            _k1(0), 
            _k2(0), 
            _k3(0), 
            _numRams((int) ceil(numInputBits / float(numRamBits))),
            _shuffling(NULL), 
            _decoders(NULL),
            _numInputBits(numInputBits),
            _numRamBits(numRamBits)
    {
        _rams = new Ram[_numRams]();
        _activations = new int[_activationsCapacity]();
        _shuffling = r.randperm(_numInputBits);
        _decoders = new Decoder[_numRams]();
        
        for (int i=0;i<_numRams;++i) 
        {
            const int start = i*_numRamBits;
            const int end   = math::min((i+1)*_numRamBits, numInputBits);
            Decoder d(_shuffling + start, end - start);
            _decoders[i] = d;
        }
    }
    
    BaseWisard(IntReader & reader) :
            _confidence(1.0), 
            _rams(NULL),
            _maxBleaching(1),
            _activations(NULL), 
            _activationsCapacity(0),
            _k1(0),
            _k2(0),
            _k3(0),
            _numRams(0),
            _shuffling(NULL),
            _decoders(NULL),
            _numInputBits(0),
            _numRamBits(0)
    {
        // Número de verificação
        int tmp = 0;
        reader.get(tmp);
        if (tmp != -1)
            throw WUPException("Invalid WiSARD file");

        // Atributos simples
        reader.get(_maxBleaching);
        reader.get(_activationsCapacity);
        reader.get(_numInputBits);
        reader.get(_numRamBits);
        reader.get(_numRams);

        int thrashSize;
        int thrashItem;
        reader.get(thrashSize);
        for (int i=0;i<thrashSize;++i)
        {
            reader.get(thrashItem);
            _thrash.push_back(thrashItem);
        }

        int targetSize;
        int inner, outter;
        reader.get(targetSize);
        for (int i=0;i<targetSize;++i) 
        {
            reader.get(inner);
            reader.get(outter);
            _innerToOutter[inner] = outter;
            _outterToInner[outter] = inner;
        }

        try 
        {
            // Aloca os vetores internos
            _activations = new int[_activationsCapacity]();
            _rams = new Ram[_numRams]();
            _shuffling = new uint[_numInputBits];
            _decoders = new Decoder[_numRams];

            if (_activations == NULL)
                throw WUPException("Out of memory");

            if (_rams == NULL)
                throw WUPException("Out of memory");

            if (_shuffling == NULL)
                throw WUPException("Out of memory");

            if (_decoders == NULL)
                throw WUPException("Out of memory");

            // Carrega o shuffling
            for (int i=0;i<_numInputBits;++i)
                _shuffling[i] = reader.get();

            // Cria as RamInputs
            for (int i=0;i<_numRams;++i)
            {
                const int start = i * _numRamBits;
                const int end   = math::min((i+1) * _numRamBits, _numInputBits);
                const int len   = end-start;

                Decoder d(_shuffling + start, len);
                _decoders[i] = d;

            }

            int length, discriminator, hits;
            std::stringstream ss;

            // Para cada ram
            for (int r=0;r<_numRams;++r) 
            {
                // Pega o input correspondente
                Decoder & decoder = _decoders[r];

                // Pega a ram correspondente (omap)
                Ram & ram = _rams[r];

                // Carrega o numero de chaves na ram
                int numKeys;
                reader.get(numKeys);
            
                // Para cada chave nesta ram
                for (int k=0;k<numKeys;++k)
                {
                    // Carrega o padrão para o input e atualiza a funcao de hash
                    for (int i=0;i<decoder.patternSize();++i)
                        reader.get(decoder.pattern()[i]);
                    decoder.updateHash();

                    // Carrega a posição endereçada
                    MultiDiscriminator &multidiscriminator = ram[decoder];

                    // Carrega o numero de discriminadores que acessaram esta posição
                    reader.get(length);
                    
                    // Carrega o conteudo desta posicao endereçada
                    for (int t=0;t<length;++t)
                    {
                        reader.get(discriminator);
                        reader.get(hits);
                        multidiscriminator[discriminator] = hits;
                    }
                }
            }
        }
        catch (WUPException e) 
        {
            delete [] _rams;
            delete [] _activations;
            delete [] _shuffling;
            delete [] _decoders;
            
            throw e;
        }
        
        // Número de verificação no final
        tmp = 0;
        reader.get(tmp);
        if (tmp != -1)
            throw WUPException("Invalid WiSARD file");
        //LOGE("_7");
    }

    ~BaseWisard()
    {
        delete [] _rams;
        delete [] _activations;
        delete [] _shuffling;
        delete [] _decoders;
    }

    void
    exportTo(const char * const filepath) const
    {
        IntFileWriter writer( filepath, 10240 );
        exportTo(writer);
    }

    void 
    exportTo(IntWriter & writer) const
    {
        // Número de verificação inicial
        int tmp = -1;
        writer.put(tmp);
        
        // Atributos simples
        writer.put(_maxBleaching);
        writer.put(_activationsCapacity);
        writer.put(_numInputBits);
        writer.put(_numRamBits);
        writer.put(_numRams);
        
        if (_innerToOutter.size() != _outterToInner.size())
            throw WUPException("Internal error");
        
        writer.put(_thrash.size());
        for (auto &n : _thrash)
            writer.put(n);
        
        writer.put(_innerToOutter.size());
        for (auto &pair : _innerToOutter) 
        {
            writer.put(pair.first);
            writer.put(pair.second);
        }
        
        for (int i=0;i<_numInputBits;++i)
            writer.put(_shuffling[i]);
        
        // Para cada ram
        for (int r=0;r<_numRams;++r) 
        {
            Ram &ram = _rams[r];
            
            int numKeys = ram.size();
            writer.put(numKeys);

//            if (r == 0)
//                LOGE("Ram %d has %d addresses", r, omap.size());
            
            // Para cada posição nela endereçada
            // int key = 0;
            for (auto it=ram.begin(); it!=ram.end();++it) {
                const MultiDiscriminator &multidiscriminator = it->second;
                
                const Decoder & ram = it->first;
                for (int i=0;i<ram.patternSize();++i)
                    writer.put(ram.pattern()[i]);
                writer.put(multidiscriminator.size());

//                if (r == 0) {
//                    std::stringstream ss;
//                    for (int i=0;i<ram.size();++i)
//                        ss << ram.pattern()[i] << " ";
//                    LOGE("RAM=0, key=%d, hash=%d, Saving %s", key++, ram.hash(), ss.str().c_str());
//                }

                // Salva as caixas
                for (auto it2=multidiscriminator.begin(); it2 != multidiscriminator.end();++it2)
                {
                    writer.put(it2->first);
                    writer.put(it2->second);
                }
            }
        }
        
        // Número de verificação final
        writer.put(tmp);
    }
    
    // The class order is not directly determined
//    const int *
//    activations() const
//    { 
//        return _activations;
//    }
    
    int 
    numDiscriminators() const
    {
        return _outterToInner.size();
    }
    
    int 
    numRamBits() const
    {
        return _numRamBits;
    }
    
    int
    numRams() const
    {
        return _numRams;
    }

    int
    numInputBits() const
    {
        return _numInputBits;
    }
    
    long 
    numPositions() const
    {
        long sum = 0;

        for (int r=0;r<_numRams;++r)
        {
            Ram &ram = _rams[r];
            for (auto it : ram)
                sum += it.second.size();
        }

        return sum;
    }

    template <typename Retina>
    int learn(const Retina & retina, int target)
    {
        target = getInnerTarget(target);

        // Redimensiona o tamanho dos vetores de classes
        if (target >= _activationsCapacity)
        {
            _activationsCapacity = target == 0 ? 2 : target * 2;
            delete [] _activations;
            _activations = new int[_activationsCapacity];
        }

        // Para cada RAM
        for (int r=0;r<_numRams;++r)
        {
            _decoders[r].read(retina);

            if (IgnoreZeroAddress && _decoders[r].isZero())
            {
                //print("Zero detected!");
                continue;
            }

//            if (r == 0) {
//                std::stringstream ss;
//                for (int i=0;i<_decoders[r].size();++i)
//                    ss << _decoders[r].pattern()[i] << " ";
//                LOGE("RAM=0 is learning a key, hash=%d, Saving %s", _decoders[r].hash(), ss.str().c_str());
//            }

            Ram &ram = _rams[r];
            MultiDiscriminator &multidiscriminator = ram[_decoders[r]];

            // Se a posição endereçada não foi alocada
            if (multidiscriminator.find(target) == multidiscriminator.end())
            {
                multidiscriminator[target] = 1;
            }
            else
            {
                multidiscriminator[target] = multidiscriminator[target] + 1;

                // Incrementa maxBleaching se necessario
                if (multidiscriminator[target] > _maxBleaching)
                    _maxBleaching = multidiscriminator[target];
            }

//            if (r == 0) {
//                auto it = omap.find(_decoders[r]);
//                auto input = it->first;
//
//                std::stringstream ss;
//                for (int i=0;i<input.size();++i)
//                    ss << input.pattern()[i] << " ";
//                LOGE("RAM0's omap has saved %s at the addressed position", ss.str().c_str());
//            }

        }

        return target;
    }

    template <typename Retina>
    int forgetSample(const Retina & retina, int target)
    {
        target = getInnerTarget(target);

        // Para cada RAM
        for (int r=0;r<_numRams;++r)
        {
            _decoders[r].read(retina);

            Ram &ram = _rams[r];
            MultiDiscriminator &multidiscriminator = ram[_decoders[r]];
            auto it = multidiscriminator.find(target);

            // Se a posição endereçada não foi alocada
            if (it == multidiscriminator.end())
                continue;

            if (it->second == 1)
                multidiscriminator.erase(it);
            else
                it->second = it->second -1;
        }

        return target;
    }

    void
    forgetClass(const int target)
    {
        auto it = _outterToInner.find(target);
        if (it == _outterToInner.end())
            return;

        const int innerTarget = it->second;

        for (int r=0;r<_numRams;++r)
        {
            Ram &ram = _rams[r];
            for (auto &pair : ram)
            {
                MultiDiscriminator &multidiscriminator = pair.second;
                auto it2 = multidiscriminator.find(innerTarget);
                if (it2 != multidiscriminator.end())
                    multidiscriminator.erase(it2);
            }
        }

        _innerToOutter.erase(it->second);
        _outterToInner.erase(it);
        _thrash.push_back(innerTarget);
    }

    template <typename Retina>
    int readCounts(const Retina &retina)
    {
        if (numDiscriminators() == 0)
        {
            _k1 = 0;
            _k2 = _k3 = -1;
            _confidence = 0.0;
            return 0;
        }

        // Limpa o vetor de ativações
        for (int i=0;i<numDiscriminators();++i)
            _activations[i] = 0;

        // Para cada RAM
        for (int r=0;r<_numRams;++r)
        {
            _decoders[r].read(retina);

            Ram &ram = _rams[r];
            const Decoder & decoder = _decoders[r];
            
            // Se não contém o endereço mapeado continua
            if (ram.find(decoder) == ram.end())
                continue;

            // Do contrario incrementa as ativações
            MultiDiscriminator &multidiscriminator = ram[decoder];
            for (auto it=multidiscriminator.begin(); it!= multidiscriminator.end();++it)
                _activations[it->first] += it->second;
        }

        // Retorna o discriminador mais ativado, calculando a confiança
        return getOutterTarget(indexOfMax(_activations, numDiscriminators()));
    }

    template <typename Retina>
    int readBinary(const Retina &retina, const int threshold=1)
    {
        if (numDiscriminators() == 0)
        {
            _k1 = 0;
            _k2 = _k3 = -1;
            _confidence = 0.0;
            return 0;
        }

        // Prepara as chaves de hash
        for (int i=0;i<_numRams;++i)
            _decoders[i].read(retina);
        
        // Equivalente ao bleaching com threshold fixo e igual a 1
        return getOutterTarget(readBleached(threshold));
    }
    
    template <typename Retina>
    int readBleaching(const Retina &retina)
    {
        return readBleaching(retina, 1, 0.0);
    }
    
    template <typename Retina>
    int readBleaching(const Retina &retina, const int step,
        const float minConfidence)
    {
        if (numDiscriminators() == 0)
        {
            _k1 = 0;
            _k2 = _k3 = -1;
            _confidence = 0.0;
            return 0;
        }

        if (step <= 0)
            throw WUPException("step must be larger than 0");
            
        if (minConfidence < 0.0 || minConfidence > 1.0)
            throw WUPException("minConfidence must be between 0.0 and 1.0");
        
        // Prepara as chaves de hash
        for (int i=0;i<_numRams;++i)
            _decoders[i].read(retina);
        
        int bestBleach       = 1;
        int bestPrediction   = 0;
        float bestConfidence = 0;

        // Aplica o bleaching
        for (int t=1;t<=_maxBleaching;t+=step)
        {
            const int predicted    = readBleached(t);
            const float confidence = getConfidence();

            // Se atingiu a confiança mínima retorne a resposta
            if (confidence > minConfidence)
                return getOutterTarget(predicted);

            // Se for a de maior confiança até agora guarde-a
            if (confidence > bestConfidence)
            {
                bestConfidence = confidence;
                bestPrediction = predicted;
                bestBleach     = t;
            }
        }

        // Se nenhum deles atingiu a confiança mínima,
        // retorne o melhor encontrado
        //LOGI("Predicted %d", bestPredicted);
        if (bestPrediction == -1)
        {
            return _innerToOutter.empty()
                    ? 0 : _innerToOutter.begin()->second;
        }
        else
        {
            // We call read bleach again to populate the activations array
            return getOutterTarget( readBleached( bestBleach ) );
        }
    }

    template <typename Retina>
    int readBinaryBleaching(const Retina &retina)
    {
        if (numDiscriminators() == 0)
        {
            _k1 = 0;
            _k2 = _k3 = -1;
            _confidence = 0.0;
            return 0;
        }

        // Prepara as chaves de hash
        for (int i=0;i<_numRams;++i)
            _decoders[i].read(retina);
        
        if (_maxBleaching == 1)
            return getOutterTarget(readBleached(1));
        
        int begin = 1;
        int end = _maxBleaching;
        int current = _nextBinaryStep(begin, end);
        
        double weightBegin = _activations[readBleached(begin)];
        double weightCurrent = _activations[readBleached(current)];
        
        double weightOne = weightBegin;
        
        while(begin != end - 1)
        {
            if (weightCurrent == weightOne)
            {
                begin = current;
                weightBegin = weightCurrent;
            } 
            else 
            {
                end = current;
            }
            
            current = _nextBinaryStep(begin, end);
            weightCurrent = _activations[readBleached(current)];
        }
        
        return getOutterTarget(getFirstBestPrediction());
    }
    
    float 
    getConfidence() const
    {
        return _confidence;
    }
    
    int
    getExcitation(const int target) const
    {
        auto it = _outterToInner.find(target);
        if (it == _outterToInner.end())
            return 0;
            //throw WUPException("Unknown target");
        
        //return _activations[it->second] / float(_numRams);
        return _activations[it->second];
    }
    
    int
    getFirstBestPrediction() const
    {
        return getOutterTarget(_k1);
    }
    
    int
    getSecondBestPrediction() const
    {
        return getOutterTarget(_k2);
    }
    
    int
    getThirdBestPrediction() const
    {
        return getOutterTarget(_k3);
    }

    bool 
    operator !=(BaseWisard const& other) const
    {
        return !(*this == other);
    }

     bool
    operator ==(BaseWisard const& other) const
    {
        if (_rams->size() != other._rams->size())
            return false;

        if (_maxBleaching != other._maxBleaching)
            return false;

        if (_activationsCapacity != other._activationsCapacity)
            return false;

        if (_numRams != other._numRams)
            return false;

        if (_numInputBits != other._numInputBits)
            return false;

        if (_numRamBits != other._numRamBits)
            return false;

        if (_thrash.size() != other._thrash.size())
            return false;

        if (_innerToOutter.size() != other._innerToOutter.size())
            return false;

        if (_outterToInner.size() != other._outterToInner.size())
            return false;

        for (int i=0;i<_numInputBits;++i)
             if (_shuffling[i] != other._shuffling[i])
                 return false;

        // TODO: Compare each ram address?

         return true;
     }

    template<typename T>
    int
    indexOfMax(const T & array, const int length)
    {
        _k1 = -1;
        _k2 = -1;
        _k3 = -1;

        for (int i=0;i<length;++i)
            if (_k1 == -1 || array[i] > array[_k1] )
                _k1 =  i;

        for (int i=0;i<length;++i)
            if (i != _k1 && (_k2 == -1 || array[i] > array[_k2]))
                _k2 = i;
        
        for (int i=0;i<length;++i)
            if (i != _k1 && i != _k2 && (_k3 == -1 || array[i] > array[_k3]))
                _k3 = i;
        
        _confidence = _k1 == -1 || _k2 == -1
                ? 1.0
                : (float) (array[_k1] - array[_k2]) / (array[_k1]);

        return _k1;
    }
    
    void
    clearActivations()
    {
        // Limpa o vetor de ativações
        for (int i=0;i<_activationsCapacity;++i)
            _activations[i] = 0;
    }

    template <typename T>
    void
    readRamBinary(const T & pattern, const int r)
    {
        readRamBleach(pattern, r, 1);
    }

    template <typename T>
    void
    readRamBleach(const T & pattern, const int r, const int threshold)
    {
        Ram &ram = _rams[r];
        const Decoder & decoder = _decoders[r];

        // Se não contém o endereço mapeado continua
        if (ram.find(decoder) == ram.end())
            return;

        // Do contrario incrementa as ativações
        MultiDiscriminator &imap = ram[decoder];
        for (auto it=imap.begin(); it!= imap.end();++it)
            if (it->second >= threshold)
                ++_activations[it->first];
    }

    const Decoder &
    decoderAt(const int index)
    {
        return _decoders[index];
    }

private:
    
    int
    _nextBinaryStep(const int begin, const int end) const
    {
        const int r = (int) round(sqrt((float) (begin * end)));
        
        if (r == begin) 
            return begin + 1;

        if (r == end) 
            return end - 1;

        return r;
    }
    
    int
    readBleached(const int threshold)
    {
        clearActivations();

        // Para cada RAM
        for (int r=0;r<_numRams;++r) 
        {
            Ram &ram = _rams[r];
            const Decoder & decoder = _decoders[r];

            // Se não contém o endereço mapeado continua
            if (ram.find(decoder) == ram.end())
                continue;

            // Do contrario incrementa as ativações
            MultiDiscriminator &imap = ram[decoder];
            for (auto it=imap.begin(); it!= imap.end();++it)
                if (it->second >= threshold)
                    ++_activations[it->first];
        }

        // Retorna o discriminador mais ativado, calculando a confiança
        return indexOfMax(_activations, _activationsCapacity);
    }
    
    int
    getInnerTarget(const int outter)
    {
        auto it = _outterToInner.find(outter);
        if (it == _outterToInner.end())
        {
            if (_thrash.empty())
            {
                const int result = _outterToInner.size();
                _outterToInner[outter] = result;
                _innerToOutter[result] = outter;
                return result;
            }
            else
            {
                const int result = _thrash.back();
                _thrash.pop_back();
                _outterToInner[outter] = result;
                _innerToOutter[result] = outter;
                return result;
            }
        }
        else
        {
            return it->second;
        }
    }

    int getInnerTarget(const int outter) const
    {
        const auto it = _outterToInner.find(outter);
        
        if (it == _outterToInner.end())
            throw WUPException("Unknown target");

        return it->second;
    }

    int
    getOutterTarget(const int inner) const
    {
        const auto it = _innerToOutter.find(inner);
        
        if (it == _innerToOutter.end())
            throw WUPException("Internal error");

        return it->second;
    }

private:

    float _confidence;
    
    Ram *_rams;
    
    int _maxBleaching;
    
    int *_activations;
    
    int _activationsCapacity;
    
    int _k1;
    
    int _k2;
    
    int _k3;

    int _numRams;
    
    uint * _shuffling;
    
    Decoder * _decoders;
    
    int _numInputBits;
    
    int _numRamBits;
    
    std::vector<int> _thrash;

    std::map<int, int> _innerToOutter;

    std::map<int, int> _outterToInner;

};

} /* wup */

#endif /* WISARD_HPP */

