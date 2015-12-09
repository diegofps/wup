#ifndef __WUP__WISARD
#define __WUP__WISARD

#include <iostream>
#include <fstream>
#include <unordered_map>

#include <wup/common.hpp>
#include "addressdecoder.hpp"

namespace wup {

using std::unordered_map;
using std::ifstream;
using std::ofstream;
using std::ostream;
using std::istream;

class Wisard {

public:
    
    Wisard(istream &file_in) : _addressDecoder(file_in)
    {
        file_in >> _numDiscriminators;
        file_in >> _numRams;
        file_in >> _maxBleaching;
        
        _output  = new int[_numDiscriminators]();
        _rams    = new unordered_map<long long, int*>[_numRams];
        _freqs   = new int[_numDiscriminators]();
        _output2 = new double[_numDiscriminators]();
        _weight  = new double[_numDiscriminators];
        _needRefresh = true;
        
        for (int i=0; i<_numDiscriminators;++i)
            file_in >> _output[i];
        
        int it, length;
        long long key;
        int *value;
        for (int i=0;i<_numRams;++i) {
            file_in >> length;
            
            for (it=0; it!=length;++it) {
                file_in >> key;
                
                value = new int[_numDiscriminators];
                for (int j=0;j<_numDiscriminators;++j)
                    file_in >> value[j];
                
                _rams[i][key] = value;
            }
        }
    }
    
    Wisard(const int inputs, const int bits, const int discriminators)
        : _addressDecoder(inputs, bits), _numDiscriminators(discriminators),
          _numRams(_addressDecoder.numRams()), 
          _output(new int[_numDiscriminators]()),
          _rams(new unordered_map<long long, int*>[_numRams]),
          _freqs(new int[_numDiscriminators]()),
          _needRefresh(true),
          _output2(new double[_numDiscriminators]()),
          _weight(new double[_numDiscriminators]),
          _maxBleaching(1),
		  _confidence(1.0)
    {
        if (bits > (int) sizeof(long long) * 8 - 1)
            warn("Number of bits entering each ram is too big!");
    }

    Wisard(const Wisard & other)
        : _addressDecoder(other._addressDecoder), 
          _numDiscriminators(other._numDiscriminators),
          _numRams(other._numRams), _output(new int[_numDiscriminators]),
          _rams(new unordered_map<long long, int*>[_numRams]),
          _freqs(new int[_numDiscriminators]()),
          _needRefresh(true),
          _output2(new double[_numDiscriminators]()),
          _weight(new double[_numDiscriminators]),
          _maxBleaching(other._maxBleaching),
		  _confidence(other._confidence)
    {
        warn("Wisard copy constructor!");
        
        memcpy(_output, other._output, sizeof(int) * (_numDiscriminators));
        memcpy(_freqs, other._freqs, sizeof(int) * (_numDiscriminators));
        
        int *tmp;
        unordered_map<long long, int*>::iterator it;
        for (int i=0;i<_numRams;++i) {
            for (it=other._rams[i].begin(); it!=other._rams[i].end();++it) {
                tmp = new int[_numDiscriminators];
                memcpy(tmp, it->second, sizeof(int) * _numDiscriminators);
                _rams[i][it->first] = tmp;
            }
        }
    }

    ~Wisard()
    {
        unordered_map<long long, int*>::iterator it;
        for (int r=0;r<_numRams;++r)
            for (it=_rams[r].begin(); it!=_rams[r].end();++it)
                delete [] it->second;

        delete [] _output2;
        delete [] _output;
        delete [] _weight;
        delete [] _freqs;
        delete [] _rams;
    }

    void learn(const int * const pattern, const int target)
    {
        if (target < 0 || target >= _numDiscriminators) {
            warn( "Invalid target ", target );
            return;
        }
        
        // Identifica as posicoes enderecadas
        const long long * const addresses = _addressDecoder.read(pattern);

        // Incremente a excitacao das posicoes enderecadas
        int * discriminators;
        for (int r=0;r<_numRams;++r) {
            unordered_map<long long, int*> &ram = _rams[r];
            unordered_map<long long, int*>::const_iterator it = ram.find(addresses[r]);

            if (it == ram.end()) {
                discriminators = new int[_numDiscriminators]();
                ram[addresses[r]] = discriminators;
            } else {
                discriminators = it->second;
            }
            
            discriminators[target] += 1;
            
            if (discriminators[target] > _maxBleaching)
                _maxBleaching = discriminators[target];
        }
        
        ++_freqs[target];
        _needRefresh = true;
    }

    int readBinary(const int * const pattern)
    {
        // Identifica as posicoes enderecadas
        const long long * const addresses = _addressDecoder.read(pattern);

        // Limpa as excitacoes anteriores
        for (int d=0;d<_numDiscriminators;++d)
            _output[d] = 0;

        // Calcula a excitacao atual
        const int * discriminators;
        for (int r=0;r<_numRams;++r) {
            const unordered_map<long long, int*> &ram = _rams[r];
            unordered_map<long long, int*>::const_iterator it = ram.find(addresses[r]);

            if (it == ram.end())
                continue;

            discriminators = it->second;
            for (int d=0;d<_numDiscriminators;++d)
                _output[d] += discriminators[d] == 0 ? 0 : 1;
        }

        // Retorna o indice do discriminador mais excitado
        return _indexOfMax(_output, 0, _numDiscriminators);
    }

    int readCounts(const int * const pattern)
    {
        // Identifica as posicoes enderecadas
        const long long * const addresses = _addressDecoder.read(pattern);

        // Limpa as excitacoes anteriores
        for (int d=0;d<_numDiscriminators;++d)
            _output[d] = 0;

        // Calcula a excitacao atual
        const int * discriminators;
        for (int r=0;r<_numRams;++r) {
            const unordered_map<long long, int*> &ram = _rams[r];
            unordered_map<long long, int*>::const_iterator it = ram.find(addresses[r]);

            if (it == ram.end())
                continue;

            discriminators = it->second;
            for (int d=0;d<_numDiscriminators;++d)
                _output[d] += discriminators[d];
        }

        // Retorna o indice do discriminador mais excitado
        return _indexOfMax(_output, 0, _numDiscriminators);
    }

    int read(const int * const pattern)
    {
    	return read_bleaching(pattern);
    }

    int readBleaching(const int * const pattern,
        const double minConf=0.1, const int step=1, const int max=1000)
    {
        return read_bleaching_limited(pattern, 0, _numDiscriminators);
    }

    int read_bleaching_limited(const int * const pattern,
    		int minTarget, int maxTarget,
			const double minConf=0.005, const int step=1, const int max=1000)
    {
    	minTarget = wup::max(0, minTarget);
    	maxTarget = wup::min(_numDiscriminators, maxTarget);

    	if (minTarget > maxTarget)
    		error("minTarget cannot be larger than maxTarget");

        const int realMax = wup::min(max, _maxBleaching);
        double bestConfidence = -2.0;
        int    bestPredicted  = minTarget;
        
        int b;
        for (b=1;b<=realMax;b+=step) {
            int predicted = _read_bleaching(pattern, b, minTarget, maxTarget);
            const double conf = confidence();

            if (conf > minConf)
                return predicted;

            if (conf > bestConfidence) {
                bestConfidence = conf;
                bestPredicted = predicted;
            }
        }

        _confidence = bestConfidence;
        return bestPredicted;
    }

    int readBleachingBalanced(const int * const pattern, 
        const int minConf=0.1, const int step=1, const int max=1000)
    {
        const int realMax = wup::min(max, _maxBleaching);
        
        if (_needRefresh)
            _refreshWeights();
        
        double bestConfidence = 0.0;
        int    bestPredicted  = 0;
        
        for (int b=1;b<=realMax;b+=step) {
            const int predicted = _read_bleaching_balanced(pattern, b);
            const double conf = confidence();
            
            if (conf > bestConfidence)
                return predicted;
            
            if (conf > bestConfidence) {
                bestConfidence = conf;
                bestPredicted = predicted;
            }
        }
        
        return bestPredicted;
    }

    int * output() const
    {
        return _output;
    }

    int classes() const
    {
        return _numDiscriminators;
    }
    
    int bits() const
    {
        return _addressDecoder.bits();
    }
    
    int inputs() const
    {
        return _addressDecoder.inputs();
    }
    
    double confidence() const
    {
        return _confidence;
    }
    
    int excitation(const int t) const
    {
        return _output[t];
    }
    
    void dumpTo(const string filename)
    {
        ofstream file_out(filename);
        if (file_out.good()) dumpTo(file_out);
        else error("Could not create wisard dump file");
    }
    
    void dumpTo(ostream &file_out)
    {
        _addressDecoder.dumpTo(file_out);
        
        file_out << " " << _numDiscriminators;
        file_out << " " << _numRams;
        file_out << " " << _maxBleaching;
        
        for (int i=0; i<_numDiscriminators;++i)
            file_out << " " << _output[i];
        
        unordered_map<long long, int*>::iterator it;
        for (int i=0;i<_numRams;++i) {
            file_out << " " << _rams[i].size();
            
            for (it=_rams[i].begin(); it!=_rams[i].end();++it) {
                file_out << " " << it->first;
                
                for (int j=0;j<_numDiscriminators;++j)
                    file_out << " " << it->second[j];
            }
        }
    }
    
    bool operator == (const Wisard & other)
    {
        if (_numDiscriminators != other._numDiscriminators) return false;
        if (_numRams != other._numRams) return false;
        if (_maxBleaching != other._maxBleaching) return false;
        
        for (int i=0;i<_numDiscriminators;++i)
            if (_output[i] != other._output[i])
                return false;
        
        if (!(_addressDecoder == other._addressDecoder)) return false;
        
        unordered_map<long long, int*>::iterator it;
        for (int i=0;i<_numRams;++i) {
            if (_rams[i].size() != other._rams[i].size())
                return false;
            
            for (it=_rams[i].begin(); it!=_rams[i].end();++it) {
                if (other._rams[i].find(it->first) == other._rams[i].end())
                    return false;
                
                int * first = it->second;
                int * second = other._rams[i][it->first];
                
                for (int j=0;j<_numDiscriminators;++j)
                    if (first[j] != second[j]) 
                        return false;
            }
        }
        
        return true;
    }
    
private:

    void _refreshWeights()
    {
        _needRefresh = false;
        const int k = indexOfMax( _freqs, _numDiscriminators );
        for (int i=0;i<_numDiscriminators;++i)
            _weight[i] = double(_freqs[k]) / _freqs[i];
    }
    
    int _read_bleaching(const int * const pattern, const int bleaching,
    		const int minTarget, const int maxTarget)
    {
        // Identifica as posicoes enderecadas
        const long long * const addresses = _addressDecoder.read(pattern);

        // Limpa as excitacoes anteriores
        for (int d=minTarget;d<maxTarget;++d)
            _output[d] = 0;

        // Calcula a excitacao atual
        const int * discriminators;
        for (int r=0;r<_numRams;++r) {
            const unordered_map<long long, int*> &ram = _rams[r];
            unordered_map<long long, int*>::const_iterator it = ram.find(addresses[r]);

            if (it == ram.end())
                continue;

            discriminators = it->second;
            for (int d=minTarget;d<maxTarget;++d)
                _output[d] += discriminators[d] >= bleaching ? 1 : 0;
        }

        // Retorna o indice do discriminador mais excitado
        return _indexOfMax(_output, minTarget, maxTarget);
    }

    int _read_bleaching_balanced(const int * const pattern, const int bleaching)
    {
        // Identifica as posicoes enderecadas
        const long long * const addresses = _addressDecoder.read(pattern);

        // Limpa as excitacoes anteriores
        for (int d=0;d<_numDiscriminators;++d)
            _output[d] = 0;

        // Calcula a excitacao atual
        const int * discriminators;
        for (int r=0;r<_numRams;++r) {
            const unordered_map<long long, int*> &ram = _rams[r];
            unordered_map<long long, int*>::const_iterator it = ram.find(addresses[r]);

            if (it == ram.end())
                continue;

            discriminators = it->second;
            for (int d=0;d<_numDiscriminators;++d)
                _output[d] += discriminators[d] >= bleaching ? 1 : 0;
        }
        
        for (int d=0;d<_numDiscriminators;++d)
            _output2[d] = _output[d] * _weight[d];
        
        // Retorna o indice do discriminador mais excitado
        return _indexOfMax(_output2, 0, _numDiscriminators);
    }

    template <typename T>
    int _indexOfMax(const T * const array, const int minTarget, const int maxTarget)
    {
        int k1 =  minTarget;
        int k2 = -1;
        
        for (int i=minTarget+1;i<maxTarget;++i)
            if (array[i] > array[k1])
                k1 =  i;
        
        for (int i=minTarget+1;i<maxTarget;++i)
            if (i != k1 && (k2 == -1 || array[i] > array[k2]))
                k2 = i;
        
        if (k2 == -1) 
            _confidence = 1.0;
        else if (array[k1] == 0.0)
			_confidence = 0.0;
        else 
            _confidence = (double) (array[k1] - array[k2]) / (array[k1]);
        
        return k1;
    }
    
private:

    wup::AddressDecoder _addressDecoder;

    int _numDiscriminators;

    int _numRams;

    int * _output;

    unordered_map<long long, int*> * _rams;

    int * _freqs;
    
    bool _needRefresh;
    
    double * _output2;
    
    double * _weight;
    
    int _maxBleaching;
    
    double _confidence;
    
};

ostream & operator<<(ostream &o, const Wisard &w)
{
    return o << "<WiSARD(Inputs=" << w.inputs()
             << ", RamBits=" << w.bits() 
             << ", Classes=" << w.classes() 
             << ")>";
}

} /* wup */

#endif /* __WUP__WISARD */

