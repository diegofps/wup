#ifndef BUNDLE_HPP
#define BUNDLE_HPP

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <wup/common/msgs.hpp>
#include <wup/common/math.hpp>
#include <wup/common/array.hpp>
#include <wup/common/parsers.hpp>
#include <wup/common/str.hpp>
#include <wup/common/generic.hpp>
#include <wup/common/io.hpp>
#include <wup/common/repr.hpp>


namespace wup {

template <typename T>
class Bundle : public wup::Repr 
{
private:

    uint _columns;
    uint _capacity;
    uint _size;
    T * _data;
    bool _ownerOfData;

public:

    Bundle() :
        _columns(0),
        _capacity(0),
        _size(0),
        _data(nullptr),
        _ownerOfData(false)
    {

    }

    Bundle(const uint columns) :
        Bundle(1, columns)
    {

    }

    Bundle(
        const uint rows, 
        const uint columns) :

        _columns(columns),
        _capacity(columns * rows),
        _size(_capacity),
        _data(new T[_capacity]),
        _ownerOfData(true)
    {

    }

    Bundle(
        const uint rows, 
        const uint columns, 
        const T & initialValue) :

        Bundle(rows, columns)
    {
        this->operator=(initialValue);
    }

    Bundle(
        T * const data, 
        const uint columns, 
        const uint size) :

        _columns(columns),
        _capacity(size),
        _size(size),
        _data(data),
        _ownerOfData(false)
    {
        if (data == nullptr && size != 0)
            throw WUPException("A bundle can't be attached to a nullptr and have a size != 0");
    }

    Bundle(Bundle<T> const & rhs) :
        _columns(rhs._columns),
        _capacity(rhs._capacity),
        _size(rhs._size),
        _data(rhs._data),
        _ownerOfData(rhs._ownerOfData)
    {
        if (rhs._ownerOfData)
        {
            _data = new T[_capacity];
            std::copy(rhs.begin(), rhs.end(), _data);
        }
    }

    Bundle(Bundle<T> && rhs) noexcept
    {
        _columns = rhs._columns;
        _capacity = rhs._capacity;
        _size = rhs._size;
        _data = rhs._data;
        _ownerOfData = rhs._ownerOfData;

        rhs._columns = 0;
        rhs._capacity = 0;
        rhs._size = 0;
        rhs._data = nullptr;
        rhs._ownerOfData = false;
    }

    Bundle(
        const std::string filename, 
        const char delimiter='\t', 
        int ignoreRows=0) :

        _columns(0),
        _capacity(0),
        _size(0),
        _data(nullptr),
        _ownerOfData(false)
    {
        // Abre o arquivo para leitura
        std::ifstream file_in(filename);
        if (!file_in.good())
            throw WUPException(cat("Could not open file: ", filename));

        // Carrega linha a linha
        int rows = 0;
        std::string line;
        std::vector<std::string> cells;

        while(!file_in.eof())
        {
            getline(file_in, line);
            ++rows;

            // Ignora cabecalhos e linhas em branco
            if (line.empty()) continue;
            if (rows <= ignoreRows) continue;

            // Quebra a linha em celulas
            str::split(line, delimiter, cells);

            // Guarda o numero de colunas
            if (_columns == 0)
                _columns = cells.size();

            // Guarda os dados
            for (uint i=0;i<cells.size();++i)
                push(parseDouble(cells[i]));
        }

        file_in.close();
    }

    void exportTo(std::string const filename) const
    {
        std::ofstream fout;
        fout.open(filename.c_str(), std::ios::out);
        fout << this->repr();
    }

    Bundle(IntReader & reader) :
        _columns(0),
        _capacity(0),
        _size(0),
        _data(nullptr),
        _ownerOfData(false)
    {
        importFrom(reader);
    }

    void
    importFrom(IntReader & reader)
    {
        uint const mode = reader.getUInt32();

        if (mode == 0)
        {
            // Nothing to do
        }
        else if (mode == 1 || mode == 2)
        {
            _columns = reader.getUInt32();
            _capacity = reader.getUInt32();
            _size = reader.getUInt32();
            _data = new T[_capacity];
            _ownerOfData = true;

            if (reader.getBool())
                reader.getData(_data, _size * sizeof(T));
        }
        else
        {
            std::string msg = cat("Invalid bundle mode: ", mode);
            throw WUPException(msg);
        }

        reader.getMilestone();
    }

    void
    exportTo(
        IntWriter & writer, 
        bool exportData=true)
    {
        if (_data == nullptr)
        {
            writer.putUInt32(0);
        }
        else if (_ownerOfData)
        {
            writer.putUInt32(1);
            writer.putUInt32(_columns);
            writer.putUInt32(_capacity);
            writer.putUInt32(_size);
            writer.putBool(exportData);

            if (exportData)
                writer.putData(_data, _size * sizeof(T));
        }
        else
        {
            writer.putUInt32(2);
            writer.putUInt32(_columns);
            writer.putUInt32(_capacity);
            writer.putUInt32(_size);
            writer.putBool(exportData);

            if (exportData)
                writer.putData(_data, _size * sizeof(T));
        }

        writer.putMilestone();
    }

    virtual ~Bundle()
    {
        if (_data == nullptr)
        {
            // Nothing to do
        }
        else if (_ownerOfData)
        {
            delete [] _data;
        }
    }

    Bundle<T> & operator=(Bundle<T> && rhs) noexcept
    {
        if (_data == nullptr)
        {
            // Nothing to do
        }
        else if (_ownerOfData)
        {
            delete [] _data;
        }

        _columns = rhs._columns;
        _capacity = rhs._capacity;
        _size = rhs._size;
        _data = rhs._data;
        _ownerOfData = rhs._ownerOfData;
        
        rhs._columns = 0;
        rhs._capacity = 0;
        rhs._size = 0;
        rhs._data = nullptr;
        rhs._ownerOfData = false;

        return *this;
    }

    bool operator==(Bundle<T> const & other) const
    {
        if (_data == nullptr)
        {
            return other._data == nullptr;
        }
        else
        {
            if (other._data == nullptr)
                return false;
            
            if (_columns != other._columns)
                return false;
            
            if (_size != other._size)
                return false;
            
            for (int i=0;i!=_size;++i)
                if (_data[i] != other._data[i])
                    return false;
            
            return true;
        }
    }

    Bundle<T> & operator=(Bundle<T> const & other)
    {
        if (_data == nullptr)
        {
            _columns = other._columns;
            _size = other._size;
            _capacity = other._size;
            _ownerOfData = true;
            _data = new T[_capacity];

            std::copy(other.begin(), other.end(), _data);
        }
        else if (_ownerOfData)
        {
            if (_capacity < other._size)
            {
                _columns = other._columns;
                _size = other._size;
                _capacity = other._size;

                delete [] _data;
                _data = new T[_capacity];
                std::copy(other.begin(), other.end(), _data);
            }
            else
            {
                _columns = other._columns;
                _size = other._size;

                std::copy(other.begin(), other.end(), _data);
            }
        }
        else
        {
            if (_size != other._size)
                throw WUPException("Bundle sizes differ");

            _columns = other._columns;
            std::copy(other.begin(), other.end(), _data);
        }

        return *this;
    }

    Bundle<T> const & operator=(const T & value)
    {
        if (_data == nullptr)
        {
            throw WUPException("Can't assign a value to bundle that is not initialized");
        }
        else
        {
            T * cur = _data;
            const T * const end = _data + _size;

            while (cur != end)
            {
                *cur = value;
                ++cur;
            }
        }

        return *this;
    }

    size_t
    index(size_t const i, size_t const j) const
    {
        return i * _columns + j;
    }

    template <typename I1, typename I2>
    T &
    operator()(const I1 & i, const I2 & j)
    {
        return _data[i * _columns + j];
    }

    template <typename I1, typename I2>
    const T &
    operator()(const I1 & i, const I2 & j) const
    {
        return _data[i * _columns + j];
    }

    T & operator()(const uint j)
    {
        return (*this)(0l, j);
    }

    const T & operator()(const uint j) const
    {
        return (*this)(0l, j);
    }

    T &
    at(const uint32_t index)
    {
        return _data[index];
    }

    const T &
    at(const uint32_t index) const
    {
        return _data[index];
    }

    void reshape(const uint cols)
    {
        reshape(1, cols);
    }

    void reshape(const uint rows, const uint cols)
    {
        if (_data == nullptr)
        {
            _columns = cols;
            _size = rows * cols;
            _capacity = _size;
            _ownerOfData = true;
            _data = new T[_capacity];
        }
        else if (_ownerOfData)
        {
            const uint newSize = rows * cols;

            _columns = cols;
            _size = newSize;

            if (newSize <= _capacity)
                return;

            delete [] _data;
            
            _capacity = newSize;
            _data = new T [_capacity];
        }
        else
        {
            if (rows * cols != _size)
                throw WUPException("This Bundle can't be reshaped to a different size. It does not own the data");

            _columns = cols;
        }
    }

    uint cols() const
    {
        return _columns;
    }

    uint rows() const
    {
        return ceil(_size / double(_columns));
    }

    void clear()
    {
        _size = 0;
    }

    void push_many(const T * const array, const uint length)
    {
        require(length);
        for (uint i=0;i<length;++i)
            _data[_size++] = array[i];
    }

    void push(const T & t, const uint times)
    {
        require(times);
        for (uint i=0;i!=times;++i)
            _data[_size++] = t;
    }

    void push(const T & t)
    {
        require(1);
        _data[_size++] = t;
    }

    T const * begin() const
    {
        return _data;
    }

    T const * end() const
    {
        return _data + _size;
    }

    T * begin()
    {
        return _data;
    }

    T * end()
    {
        return _data + _size;
    }

    /*
        Pointer to the first element
    */
    T * data()
    {
        return _data;
    }

    T const * data() const
    {
        return _data;
    }

    /*
        Returns the number of elements in this Bundle
    */
    uint size() const
    {
        return _size;
    }

    // String representation of the data in this bundle
    virtual std::string repr() const
    {
        std::stringstream ss;

        for (uint i=0;i<rows();++i)
        {
            ss << this->operator()(i, uint(0));
            for (uint j=1;j!=cols();++j)
                ss << "," << this->operator()(i, j);
            ss << "\n";
        }

        return ss.str();
    }

private:

    void
    require(int const quantity)
    {
        if (_data == nullptr)
        {
            _size = quantity;
            _capacity = _size;
            _ownerOfData = true;
            _data = new T[_capacity];
        }
        else if (_ownerOfData)
        {
            if (_size + quantity <= _capacity)
                return;

            int newCap = math::max(_capacity + quantity, _capacity * 2);
            T * newData = new T[newCap];

            std::copy(_data, _data + _size, newData);
            delete [] _data;

            _capacity = newCap;
            _data = newData;
        }
        else
        {
            throw WUPException("This Bundle can't be resized. It does not own the data");
        }
    }

};

template <typename T>
T
min(Bundle<T> const & data)
{
    return arr::min(&data(0,0), data.size());
}

template <typename T>
T
max(Bundle<T> const & data)
{
    return arr::max(&data(0,0), data.size());
}

}
#endif // BUNDLE_HPP
