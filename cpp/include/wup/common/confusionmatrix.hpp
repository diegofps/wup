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

    void
    checkUpdate()
    {
        if (_requiresUpdate)
            update();
    }

public:

    ConfusionMatrix(const Dataset &ds) :
        _matrix(ds.classes()+1, ds.classes()+1),
        _requiresUpdate(true)
    {

    }

    void
    import(ConfusionMatrix & other)
    {
        if (classes() != other.classes())
            throw WUPException("Can not digest a ConfusionMatrix with different size");

        for (int i=0;i<classes();++i)
            for (int j=0;j<classes();++j)
                _matrix(i, j) += other(i, j);

        _requiresUpdate = true;
    }

    int
    classes() const
    {
        return _matrix.numCols() - 1;
    }

    void
    clear()
    {
        _matrix = 0;
        _requiresUpdate = true;
    }

    int
    predictionsFor(const int row) const
    {
        int sum = 0;
        for (uint j=0;j<_matrix.numCols()-1;++j)
            sum += _matrix(row, j);
        return sum;
    }

    int
    targetsFor(const int col) const
    {
        int sum = 0;
        for (uint i=0;i<_matrix.numRows()-1;++i)
            sum += _matrix(i, col);
        return sum;
    }

    ulong
    total() const
    {
        ulong sum = 0;
        for (uint i=0;i<_matrix.numRows()-1;++i)
            sum += predictionsFor(i);
        return sum;
    }

    void
    add(const int predicted, const int target)
    {
        ++_matrix(predicted, target);
        _requiresUpdate = true;
    }

    double
    accuracy()
    {
        checkUpdate();
        const int len = _matrix.numCols()-1;
        return _matrix(len, len);
    }

    int
    max() const
    {
        int max = -1;
        for (int i=0;i<classes();++i) {
            for (int j=0;j<classes();++i) {
                const int value = _matrix(i,j);
                if (value > max)
                    max = value;
            }
        }
        return max;
    }

    int
    diagonal() const
    {
        int sum = 0;
        for (int k=0;k<classes();++k)
            sum += _matrix(k, k);
        return sum;
    }

    double
    operator()(const int i, const int j)
    {
        checkUpdate();
        return _matrix(i, j);
    }

    void
    update()
    {
        const int len = _matrix.numCols()-1;

        for (int k=0;k<len;++k)
        {
            const double current = _matrix(k, k);
            const double rowSum = predictionsFor(k);
            const double colSum = targetsFor(k);

            _matrix(len, k) = colSum == 0 ? 1.0 : current / colSum;
            _matrix(k, len) = rowSum == 0 ? 1.0 : current / rowSum;
        }

        const ulong sum = total();
        const double diag = diagonal();
        _matrix(len, len) = sum == 0 ? 1.0 : diag / sum;
        _requiresUpdate = false;
    }

};

#ifndef JNI_H_
std::ostream & operator << (std::ostream &o, ConfusionMatrix & confusion)
{
    o << YELLOW;
    for (int j=0; j<confusion.classes(); ++j)
        o << '\t' << j;
    o << std::endl;

    for (int i=0;i<confusion.classes();++i) {
        o << BLUE << i << '\t' << (i==0?GREEN:confusion(i, 0)==0?NORMAL:RED) << confusion(i, 0);

        for (int j=1;j<confusion.classes();++j)
            o << '\t' << (i==j?GREEN:confusion(i, j)==0?NORMAL:RED) << confusion(i, j);

        o << std::fixed << std::setprecision(3);
        const double value = confusion(i, confusion.classes());
        o << '\t' << (value == 1.0 ? GREEN : value < .5 ? RED : BLUE) << value << std::endl;
        o << std::defaultfloat;
    }

    o << BLUE << std::fixed << std::setprecision(3);
    for (int j=0; j<=confusion.classes(); ++j) {
        const double value = confusion(confusion.classes(), j);
        o << '\t' << (value == 1.0 ? GREEN : value < .5 ? RED : BLUE) << value;
    }

    return o << std::defaultfloat << std::endl << NORMAL;
}
#endif

}

#endif // CONFUSIONMATRIX_HPP
