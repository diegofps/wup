#ifndef __WUP__TOPK
#define __WUP__TOPK

#include <exception>
#include <iostream>
#include <cstring>

using std::ostream;
using std::istream;

namespace wup {

template <typename W, typename T>
class TopK {
public:


    class TopKException : public std::exception {
        public:
        TopKException(const char * const msg) : _msg(msg) { }
        virtual const char * what() const throw() { return _msg; }
        const char * _msg;
    };


    class Node {
    public:

        Node() { }
        
        Node(const W w, const T d) : weight(w), data(d) { }
        
        void dumpTo(ostream &file_out)
        {
            file_out << " " << weight;
            file_out << " " << data;
        }
        
        void loadFrom(istream &file_in)
        {
            file_in >> weight;
            file_in >> data;
        }
        
        bool operator != (const Node &other) const
        {
            return !((*this) == other);
        }
        
        bool operator == (const Node &other) const
        {
            if (weight != other.weight) return false;
            if (data != other.data) return false;
            return true;
        }
    
    public:
    
        W weight;
        
        T data;
    };

    
    class iterator {
    public:
        iterator(const Node * const mem, const int pos)
            : _mem(mem), _pos(pos) { }

        const T operator*() const
        {
            return _mem[_pos].data;
        }

        void operator++()
        {
            ++_pos;
        }

        bool operator!=(const iterator &other) const
        {
            return _pos != other._pos;
        }

    private:
        const Node * const _mem;
        int _pos;
    };
    
public:

    TopK(istream &file_in)
    {
        file_in >> _k;
        file_in >> _eltos;

        _mem = new Node[_k];

        for (int i=0;i<_eltos;++i)
            _mem[i].loadFrom(file_in);
    }

    TopK(const int k) : _k(k), _eltos(0), _mem(new Node[k])
    {

    }

    TopK(const TopK &other) : _k(other._k), _eltos(other._eltos), _mem(new Node[_k])
    {
        memcpy(_mem, other._mem, _eltos * sizeof(Node));
    }

    ~TopK()
    {
        delete [] _mem;
    }
    
    void push(const W &weight, const T &data)
    {
        if (_eltos == _k) {
            if (_mem[0].weight < weight) {
                _mem[0] = Node(weight, data);
                goDown(0);
            }
        } else {
            _mem[_eltos] = Node(weight, data);
            goUp(_eltos++);
        }
    }
    
    T pop()
    {
        W tmp;
        return pop(tmp);
    }
    
    T pop(W &weight)
    {
        if (_eltos == 0)
            throw TopKException("TopK is empty.");
        
        weight = _mem[0].weight;
        T temp = _mem[0].data;
        _mem[0] = _mem[_eltos-1];
        _eltos--;
        goDown(0);
        return temp;
    }
    
    int size() const
    {
        return _eltos;
    }

    bool hasAny() const
    {
        return _eltos != 0;
    }
    
    bool isFull() const
    {
        return _eltos == _k;
    }
    
    int k() const
    {
        return _k;
    }
    
    void clear()
    {
        _eltos = 0;
    }
    
    W base_weight() const
    {
        if (_eltos == 0)
            throw TopKException("TopK is empty.");
        
        return _mem[0].weight;
    }
    
    T base_data() const
    {
        if (_eltos == 0)
            throw TopKException("TopK is empty.");
        
        return _mem[0].data;
    }
    
    void dumpTo(ostream &file_out)
    {
        file_out << " " << _k;
        file_out << " " << _eltos;

        for (int i=0;i<_eltos;++i)
            _mem[i].dumpTo(file_out);
    }
    
    
    bool operator != (const TopK<W, T> &other) const
    {
        return !((*this) == other);
    }
    
    bool operator == (const TopK<W, T> &other) const
    {
        if (_k != other._k) return false;
        if (_eltos != other._eltos) return false;

        for (int i=0;i<_eltos;++i)
            if (_mem[i] != other._mem[i])
                return false;

        return true;
    }
    
    iterator begin() const
    {
        return iterator(_mem, 0);
    }
    
    iterator end() const
    {
        return iterator(_mem, _eltos);
    }

private:
    
    void goUp(int i) 
    {
        Node temp;
        int parentId = parent(i);
        
        while(i != 0 && _mem[parentId].weight > _mem[i].weight) {
            temp = _mem[parentId];
            _mem[parentId] = _mem[i];
            _mem[i] = temp;
            i = parentId;
            parentId = parent(i);
        }
    }
    
    void goDown(int i)
    {
        Node temp;
        for (;;) {
            Node &current = _mem[i];

            if (hasRight(i)) {
                Node &leftSon = _mem[left(i)];
                Node &rightSon = _mem[right(i)];

                if (leftSon.weight < rightSon.weight) {
                    if (leftSon.weight < current.weight) {
                        temp = current;
                        current = leftSon;
                        leftSon = temp;
                        i = left(i);
                    } else break;
                } else {
                    if (rightSon.weight < current.weight) {
                        temp = current;
                        current = rightSon;
                        rightSon = temp;
                        i = right(i);
                    } else break;
                }
            } else if (hasLeft(i)) {
                Node &leftSon = _mem[left(i)];

                if (leftSon.weight < current.weight) {
                    temp = current;
                    current = leftSon;
                    leftSon = temp;
                    i = left(i);
                } else break;
            } else break;
        }
    }
    
    int parent(const int i) const
    {
        return (i-1)/2;
    }
    
    int left(const int i) const
    {
        return i*2+1;
    }
    
    int right(const int i) const
    {
        return i*2+2;
    }
    
    bool hasLeft(const int i) const 
    {
        return left(i) < _eltos;
    }
    
    bool hasRight(const int i) const 
    {
        return right(i) < _eltos;
    }
    
private:
    
    int _k;
    
    int _eltos;
    
    Node *_mem;
    
};

} /* wup */

#endif


