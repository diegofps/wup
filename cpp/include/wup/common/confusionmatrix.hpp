#ifndef CONFUSIONMATRIX_HPP
#define CONFUSIONMATRIX_HPP

#include <iomanip>
#include <iostream>

#include <wup/common/bundle.hpp>
#include <wup/common/dataset.hpp>

namespace wup {

class ConfusionMatrix {
private:

    Bundle<double> _matrix;

    bool _requiresUpdate;

public:

    ConfusionMatrix(const uint numClasses) :
        _matrix(numClasses+1, numClasses+1),
        _requiresUpdate(true)
    {
        _matrix = 0;
    }

    void
    import(ConfusionMatrix & other)
    {
        if (classes() != other.classes())
            throw WUPException("Can not digest a ConfusionMatrix with different size");

        for (uint i=0;i!=classes();++i)
            for (uint j=0;j!=classes();++j)
                _matrix(i, j) += other(i, j);

        _requiresUpdate = true;
    }

    uint
    classes() const
    {
        return _matrix.cols() - 1;
    }

    void
    clear()
    {
        _matrix = 0;
        _requiresUpdate = true;
    }

    uint64_t
    predictionsFor(const uint row) const
    {
        uint64_t sum = 0;
        for (uint j=0;j<_matrix.cols()-1;++j)
            sum += _matrix(row, j);
        return sum;
    }

    int
    targetsFor(const uint col) const
    {
        int sum = 0;
        for (uint i=0;i<_matrix.rows()-1;++i)
            sum += _matrix(i, col);
        return sum;
    }

    uint64_t
    total() const
    {
        uint64_t sum = 0;
        for (uint i=0;i<_matrix.rows()-1;++i)
            sum += predictionsFor(i);
        return sum;
    }

    void
    add(const int64_t predicted, const int64_t target)
    {
        ++_matrix(predicted, target);
        _requiresUpdate = true;
    }

    double
    accuracy()
    {
        checkUpdate();
        const uint len = _matrix.cols()-1;
        return _matrix(len, len);
    }

    int
    maxValue() const
    {
        int best = -1;
        for (uint i=0;i!=classes();++i)
        {
            for (uint j=0;j!=classes();++i)
            {
                const int value = static_cast<int>(_matrix(i,j));

                if (value > best)
                    best = value;
            }
        }
        return best;
    }

    int
    diagonal() const
    {
        int sum = 0;
        for (uint k=0;k!=classes();++k)
            sum += _matrix(k, k);
        return sum;
    }

    double
    operator()(uint const i, uint const j)
    {
        checkUpdate();
        return _matrix(i, j);
    }

    double
    operator()(uint const i, uint const j) const
    {
        return _matrix(i, j);
    }

    ConfusionMatrix &
    update()
    {
        const uint len = _matrix.cols()-1;

        for (uint k=0;k!=len;++k)
        {
            const double current = _matrix(k, k);
            const double rowSum = predictionsFor(k);
            const double colSum = targetsFor(k);

            _matrix(len, k) = colSum < 0.0001 ? 1.0 : current / colSum;
            _matrix(k, len) = rowSum < 0.0001 ? 1.0 : current / rowSum;
        }

        const uint64_t sum = total();
        const double diag = diagonal();
        _matrix(len, len) = sum == 0 ? 1.0 : diag / sum;
        _requiresUpdate = false;

        return *this;
    }

private:

    void
    checkUpdate()
    {
        if (_requiresUpdate)
            update();
    }

};

#ifndef JNI_H_
inline std::ostream &
operator << (std::ostream & o,
             const ConfusionMatrix & confusion)
{
    o << YELLOW;
    for (uint j=0; j!=confusion.classes(); ++j)
        o << '\t' << j;
    o << std::endl;

    for (uint i=0;i!=confusion.classes();++i) {
        o << BLUE << i << '\t' << (i==0?GREEN:confusion(i, 0)<0.001?NORMAL:RED) << confusion(i, 0);

        o << std::defaultfloat;

        for (uint j=1;j!=confusion.classes();++j)
            o << '\t' << (i==j?GREEN:confusion(i, j)<0.001?NORMAL:RED) << confusion(i, j);

        o << std::fixed << std::setprecision(4);

        const double value = confusion(i, confusion.classes());

        o << '\t' << (value == 1.0 ? GREEN : value < .5 ? RED : BLUE) << value << std::endl;
        o << std::defaultfloat;
    }

    o << BLUE << std::fixed << std::setprecision(3);

    for (uint j=0; j<=confusion.classes(); ++j) {
        const double value = confusion(confusion.classes(), j);
        o << '\t' << (value == 1.0 ? GREEN : value < .5 ? RED : BLUE) << value;
    }

    return o << std::defaultfloat << std::endl << NORMAL;
}
#endif

}

#endif // CONFUSIONMATRIX_HPP
