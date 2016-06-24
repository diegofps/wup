#ifndef WISARD_HPP
#define WISARD_HPP

#include <unordered_map>
#include <map>

#include "../common/sbio.hpp"
#include "binaryinput.hpp"
#include "factorialinput.hpp"
#include "pattern.hpp"

namespace wup {

template <typename RamInput=BinaryInput>
class Wisard {
public:
    
    typedef std::map<int, int> innermap;
    typedef std::unordered_map<RamInput, innermap> outtermap;

    Wisard(const int inputBits, const int ramBits) : 
            Wisard(inputBits, ramBits, 2)
        { }
    
    Wisard(const int inputBits, const int ramBits, const int classes) : 
            _confidence(1.0), 
            _memory(NULL), 
            _maxBleaching(1), 
            _activations(NULL), 
            _numDiscriminators(classes), 
            _k1(0), 
            _k2(0), 
            _k3(0), 
            _numRams((int) ceil(inputBits / float(ramBits))), 
            _shuffling(NULL), 
            _ramInputs(NULL), 
            _inputBits(inputBits), 
            _ramBits(ramBits)
    {
        _memory = new outtermap[_numRams]();
        _activations = new int[_numDiscriminators]();
        _shuffling = randperm(_inputBits);
        _ramInputs = new RamInput[_numRams]();
        
        for (int i=0;i<_numRams;++i) {
            const int start = i*_ramBits;
            const int end   = min((i+1)*_ramBits, inputBits);
            _ramInputs[i] = RamInput(&_shuffling[start], end-start);
        }
    }
    
    Wisard(sbreader<int> &reader) : 
            _confidence(1.0), 
            _memory(NULL),
            _maxBleaching(1),
            _activations(NULL), 
            _numDiscriminators(0),
            _k1(0),
            _k2(0),
            _k3(0),
            _numRams(0),
            _shuffling(NULL),
            _ramInputs(NULL),
            _inputBits(0),
            _ramBits(0)
    {
        // Número de verificação
        int tmp = 0;
        reader.get(tmp);
        if (tmp != -1)
            throw WUPException("Illegal or corrupted WiSARD file");
        
        // Atributos simples
        reader.get(_maxBleaching);
        reader.get(_numDiscriminators);
        reader.get(_inputBits);
        reader.get(_ramBits);
        reader.get(_numRams);
        
        int thrashSize;
        int thrashItem;
        reader.get(thrashSize);
        for (int i=0;i<thrashSize;++i) {
            reader.get(thrashItem);
            _thrash.push_back(thrashItem);
        }
        
        int targetSize;
        int inner, outter;
        reader.get(targetSize);
        for (int i=0;i<targetSize;++i) {
            reader.get(inner);
            reader.get(outter);
            _innerToOutter[inner] = outter;
            _outterToInner[outter] = inner;
        }
        
        try {
        
            // Aloca os vetores internos
            _activations = new int[_numDiscriminators]();
            _memory = new outtermap[_numRams]();
            _shuffling = new int[_inputBits];
            _ramInputs = new RamInput[_numRams];
            int * tmp = new int[_ramBits];
            
            // carrega o shuffling e cria os RamInputs
            for (int i=0;i<_inputBits;++i)
                reader.get(_shuffling[i]);
            
            for (int i=0;i<_numRams;++i)
                _ramInputs[i] = RamInput(&_shuffling[i*_ramBits], _ramBits);
            
            int length, target, hits;
            RamInput key(NULL, _ramBits);
            
            // Para cada ram
            for (int r=0;r<_numRams;++r) {
                int keys;
                reader.get(keys);
                outtermap &omap = _memory[r];
                //LOGE("I: Ram %d has %d STARTED", r, keys);
            
                // Para cada chave nesta ram
                for (int k=0;k<keys;++k) {
                
                    for (int i=0;i<_ramBits;++i)
                        reader.get(tmp[i]);
                    
                    key.setInput(tmp);
                    reader.get(length);
                    
                    //LOGE("I: Ram %d Key %d", r, key);
                    innermap &imap = omap[key];
                    for (int t=0;t<length;++t) {
                        reader.get(target);
                        reader.get(hits);
                        imap[target] = hits;
                    }
                }
                
                //LOGE("I: Ram %d has %d ENDED", r, keys);
            }
            
            delete [] tmp;
            // Se algo der errado limpe os vetores
        } catch (WUPException e) {
            delete [] _memory;
            delete [] _activations;
            delete [] _shuffling;
            delete [] _ramInputs;
            
            throw e;
        }
        
        // Número de verificação no final
        tmp = 0;
        reader.get(tmp);
        if (tmp != -1)
            throw WUPException("Illegal or corrupted WiSARD file");
    }

    ~Wisard()
    {
        delete [] _memory;
        delete [] _activations;
        delete [] _shuffling;
        delete [] _ramInputs;
    }

    void
    exportTo(const char * const filename) const
    {
        sbwriter<int> writer(filename, 10240);
        exportTo(writer);
    }

    void 
    exportTo(sbwriter<int> &writer) const
    {
        // Número de verificação inicial
        int tmp = -1;
        writer.put(tmp);
        
        // Atributos simples
        writer.put(_maxBleaching);
        writer.put(_numDiscriminators);
        writer.put(_inputBits);
        writer.put(_ramBits);
        writer.put(_numRams);
        
        if (_innerToOutter.size() != _outterToInner.size())
            throw WUPException("Internal error");
        
        writer.put(_thrash.size());
        for (auto &n : _thrash)
            writer.put(n);
        
        writer.put(_innerToOutter.size());
        for (auto &pair : _innerToOutter) {
            writer.put(pair.first);
            writer.put(pair.second);
        }
        
        for (int i=0;i<_inputBits;++i)
            writer.put(_shuffling[i]);
        
        // Para cada ram
        for (int r=0;r<_numRams;++r) {
            outtermap &omap = _memory[r];
            
            int keys = omap.size();
            writer.put(keys);
            //LOGE("E: Ram %d has %d", r, _memory[r].size());
            
            // Para cada posição nela endereçada
            typename outtermap::const_iterator it;
            for (it=omap.begin(); it!=omap.end();++it) {
                const innermap &imap = it->second;
                
                for (int i=0;i<_ramBits;++i)
                    writer.put(it->first.pattern()[i]?1:0);
                writer.put(imap.size());
                
                //LOGE("E: Ram %d Key %lld", r, it->first);
                
                // Salva as caixas
                innermap::const_iterator it2;
                for (it2=imap.begin(); it2 != imap.end();++it2) {
                    writer.put(it2->first);
                    writer.put(it2->second);
                }
            }
        }
        
        // Número de verificação final
        writer.put(tmp);
    }
    
    const int *
    activations() const
    { return _activations; }
    
    int 
    classes() const
    { return _numDiscriminators; }
    
    int 
    ramBits() const
    { return _ramBits; }
    
    int
	numRams() const
    { return _numRams; }

    int
    inputBits() const
    { return _inputBits; }
    
    int
    learn(const Pattern & pattern, int target)
    {
    	target = getInnerTarget(target);

        // Prepara as chaves de hash
        for (int i=0;i<_numRams;++i)
            _ramInputs[i].read(pattern);
        
        // Redimensiona o tamanho dos vetores de classes
		if (target >= _numDiscriminators) {
			_numDiscriminators = target * 2;
			delete [] _activations;
			_activations = new int[_numDiscriminators];
		}
		
		// Para cada RAM
		for (int r=0;r<_numRams;++r) {
			outtermap &omap = _memory[r];
			innermap &imap = omap[_ramInputs[r]];
			
			// Se a posição endereçada não foi alocada
			if (imap.find(target) == imap.end()) {
			    imap[target] = 1;
			} else {
			    imap[target] = imap[target] + 1;
			
			    // Incrementa maxBleaching se necessario
			    if (imap[target] > _maxBleaching)
			        _maxBleaching = imap[target];
	        }
		}

		return target;
    }

    void
	forget(const int target)
    {
    	auto it = _outterToInner.find(target);
    	if (it == _outterToInner.end())
    		return;

    	const int realTarget = it->second;

    	for (int r=0;r<_numRams;++r) {
    		outtermap &omap = _memory[r];
    		for (auto &pair : omap) {
    			innermap &imap = pair.second;
    			auto it2 = imap.find(realTarget);
    			if (it2 != imap.end())
    				imap.erase(it2);
    		}
    	}

    	_innerToOutter.erase(it->second);
    	_outterToInner.erase(it);
    	_thrash.push_back(realTarget);
    }

	int 
	readCounts(const Pattern &pattern)
	{
        // Prepara as chaves de hash
        for (int i=0;i<_numRams;++i)
            _ramInputs[i].read(pattern);
        
		// Limpa o vetor de ativações
		for (int i=0;i<_numDiscriminators;++i)
			_activations[i] = 0;

		// Para cada RAM
		for (int r=0;r<_numRams;++r) {
			outtermap &omap = _memory[r];
            const RamInput & rIn = _ramInputs[r];
            
			// Se não contém o endereço mapeado continua
			if (omap.find(rIn) == omap.end())
				continue;

			// Do contrario incrementa as ativações
			innermap::const_iterator it;
			innermap &imap = omap[rIn];
			for (it=imap.begin(); it!= imap.end();++it)
				_activations[it->first] += it->second;
		}

		// Retorna o discriminador mais ativado, calculando a confiança
		return getOutterTarget(indexOfMax(_activations, _numDiscriminators));
	}

    int 
    readBinary(const Pattern &pattern) 
    {
        // Prepara as chaves de hash
        for (int i=0;i<_numRams;++i)
            _ramInputs[i].read(pattern);
        
        // Equivalente ao bleaching com threshold fixo e igual a 1
        return getOutterTarget(readThreshold(1));
    }
    
    int 
    readBleaching(const Pattern &pattern)
    { return readBleaching(pattern, 1, 0.1); }
    
    int 
    readBleaching(const Pattern &pattern, const int step, 
        const float minConfidence)
    {
        if (step <= 0)
            throw WUPException("step must be larger than 0");
        if (minConfidence < 0.0 || minConfidence > 1.0)
            throw WUPException("minConfidence must be between 0.0 and 1.0");
        
        // Prepara as chaves de hash
        for (int i=0;i<_numRams;++i)
            _ramInputs[i].read(pattern);
        
		int bestPredicted    = 0;
		float bestConfidence = 0;

		// Aplica o bleaching
		for (int t=1;t<=_maxBleaching;t+=step) {
			const int predicted    = readThreshold(t);
			const float confidence = getConfidence();
			
			// Se atingiu a confiança mínima retorne a resposta
			if (confidence > minConfidence)  {
				return getOutterTarget(predicted);
			}
			
			// Se for a de maior confiança até agora guarde-a
			if (confidence > bestConfidence) {
				bestConfidence = confidence;
				bestPredicted  = predicted;
			}
		}
		
		// Se nenhum deles atingiu a confiança mínima, 
		// retorne o melhor encontrado
	    //LOGI("Predicted %d", bestPredicted);
	    if (bestPredicted == -1) 
	        return _innerToOutter.empty()?0:_innerToOutter.begin()->second;
        else
    		return getOutterTarget(bestPredicted);
    }

    int 
    readBinaryBleaching(const Pattern &pattern)
    {
        // Prepara as chaves de hash
        for (int i=0;i<_numRams;++i)
            _ramInputs[i].read(pattern);
        
        if (_maxBleaching == 1)
            return getOutterTarget(readThreshold(1));
        
        int begin = 1;
        int end = _maxBleaching;
        int current = _nextBinaryStep(begin, end);
        
        double weightBegin = _activations[readThreshold(begin)];
        double weightCurrent = _activations[readThreshold(current)];
        
        double weightOne = weightBegin;
        
        while(begin != end - 1) {
            if (weightCurrent == weightOne) {
                begin = current;
                weightBegin = weightCurrent;
            } else {
                end = current;
            }
            
            current = _nextBinaryStep(begin, end);
            weightCurrent = _activations[readThreshold(current)];
        }
        
        return getOutterTarget(getFirstBestPrediction());
    }
    
    float 
    getConfidence() const
    { return _confidence; }
    
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
    { return getOutterTarget(_k1); }
    
    int 
    getSecondBestPrediction() const
    { return getOutterTarget(_k2); }
    
    int
    getThirdBestPrediction() const
    { return getOutterTarget(_k3); }

    bool operator !=(Wisard const& other) const {
    	return !(*this == other);
    }

 	bool operator ==(Wisard const& other) const {
 		if (_memory->size() != other._memory->size()) return false;
 		if (_maxBleaching != other._maxBleaching) return false;
 		if (_numDiscriminators != other._numDiscriminators) return false;
 		if (_numRams != other._numRams) return false;
 		if (_inputBits != other._inputBits) return false;
 		if (_ramBits != other._ramBits) return false;
 		if (_thrash.size() != other._thrash.size()) return false;
 		if (_innerToOutter.size() != other._innerToOutter.size()) return false;
 		if (_outterToInner.size() != other._outterToInner.size()) return false;

 		for (int i=0;i<_inputBits;++i)
 			if (_shuffling[i] != other._shuffling[i])
 				return false;

 		return true;
 	}

private:
    
    int
    _nextBinaryStep(const int begin, const int end) const
    {
        const int r = (int) round(sqrt((float) (begin * end)));
        if (r == begin) return begin + 1;
        if (r == end) return end - 1;
        return r;
    }
    
    int
    readThreshold(const int threshold)
    {
		// Limpa o vetor de ativações
		for (int i=0;i<_numDiscriminators;++i)
			_activations[i] = 0;
		
		// Para cada RAM
		for (int r=0;r<_numRams;++r) {
			outtermap &omap = _memory[r];
            const RamInput & rIn = _ramInputs[r];
			
			// Se não contém o endereço mapeado continua
			if (omap.find(rIn) == omap.end())
			    continue;
		    
		    // Do contrario incrementa as ativações
			innermap::const_iterator it;
			innermap &imap = omap[rIn];
			for (it=imap.begin(); it!= imap.end();++it)
			    if (it->second >= threshold)
			        ++_activations[it->first];
		}
		
		// Retorna o discriminador mais ativado, calculando a confiança 
		return indexOfMax(_activations, _numDiscriminators);
    }
    
    int 
    indexOfMax(const int * const array, const int length)
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
        
		if (_k1 == -1 || _k2 == -1) 
			_confidence = 1.0;
		else 
			_confidence = (float) (array[_k1] - array[_k2]) / (array[_k1]);
		
		return _k1;
	}
    
    int
	getInnerTarget(const int outter)
    {
    	auto it = _outterToInner.find(outter);
    	if (it == _outterToInner.end()) {
    		if (_thrash.empty()) {
    			const int result = _outterToInner.size();
    			_outterToInner[outter] = result;
    			_innerToOutter[result] = outter;
    			return result;
    		} else {
    			const int result = _thrash.back();
    			_thrash.pop_back();
    			_outterToInner[outter] = result;
    			_innerToOutter[result] = outter;
    			return result;
    		}
    	} else {
    		return it->second;
    	}
    }

    int getInnerTarget(const int outter) const
    {
        const auto it = _outterToInner.find(outter);
        if (it != _outterToInner.end())
            return it->second;
        else
            throw WUPException("Unknown target");
    }

    int
	getOutterTarget(const int inner) const
    {
        const auto it = _innerToOutter.find(inner);
        if (it != _innerToOutter.end())
            return it->second;
        else
            throw WUPException("Internal error");
    }

private:

    float _confidence;
    
    outtermap *_memory;
    
    int _maxBleaching;
    
    int *_activations;
    
    int _numDiscriminators;
    
    int _k1;
    
    int _k2;
    
    int _k3;

    int _numRams;
    
    int * _shuffling;
    
    RamInput * _ramInputs;
    
    int _inputBits;
    
    int _ramBits;
    
    std::vector<int> _thrash;

    std::map<int, int> _innerToOutter;

    std::map<int, int> _outterToInner;

};

} /* wup */

#endif /* WISARD_HPP */

