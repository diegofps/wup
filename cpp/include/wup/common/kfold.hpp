#ifndef KFOLD_HPP
#define KFOLD_HPP

#include <unordered_map>
#include <iterator>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include <wup/common/dataset.hpp>
#include <wup/common/msgs.hpp>

namespace wup {

class KFold;

class Fold {

private:
    friend class KFold;

    ref_vector<const Sample> _trainingSamples;

    ref_vector<const Sample> _testingSamples;

    int _id;

    const Dataset * _ds;

    void
    addTrainingSample(const Sample &sample)
    {
        _trainingSamples.push_back(&sample);
    }

    void
    addTestingSample(const Sample &sample)
    {
        _testingSamples.push_back(&sample);
    }

public:

    Fold() : _id(-1)
    {

    }

    Fold(int id) : _id(id)
    {

    }

    const ref_vector<const Sample> &
    trainingSamples() const
    {
        return _trainingSamples;
    }

    const ref_vector<const Sample> &
    testingSamples() const
    {
        return _testingSamples;
    }

    int id() const
    {
        return _id;
    }

    void id(int id)
    {
        _id = id;
    }

    const Dataset & dataset() const
    {
        return *_ds;
    }

    void dataset(const Dataset & ds)
    {
        _ds = &ds;
    }
};

class KFold {
private:

    const Dataset * _ds;

    uint _numFolds;

    Fold * _folds;

    const char * INCOMPATIBLE_FILE_MESSAGE =
            "This file is not a KFold file or it is compatible with this dataset";

    void
    createFolds(const int numFolds)
    {
        _folds = new Fold[numFolds];
        for (int i=0; i<numFolds; ++i)
        {
            _folds[i].id(i);
            _folds[i].dataset(*_ds);
        }
    }

    void
    loadFromStream(const Dataset &ds, std::istream &stream)
    {
        if (!stream.good())
            throw WUPException("Could not restore KFold from file");

        uint lengthTrain;
        uint lengthTest;
        ulong id;

        stream >> _numFolds;
        if (stream) createFolds(_numFolds);

        for (uint f=0;f<_numFolds;++f)
        {
            stream >> lengthTrain;
            stream >> lengthTest;

            for (uint j=0;j<lengthTrain;++j)
            {
                stream >> id;
                if (id >= ds.size())
                    throw WUPException(INCOMPATIBLE_FILE_MESSAGE);
                _folds[f].addTrainingSample(ds[id]);
            }

            for (uint j=0;j<lengthTest;++j)
            {
                stream >> id;
                if (id >= ds.size())
                    throw WUPException(INCOMPATIBLE_FILE_MESSAGE);
                _folds[f].addTestingSample(ds[id]);
            }
        }

        ulong total = 0;
        for (uint i=0;i<_numFolds;++i)
            total += _folds[i].trainingSamples().size() +
                     _folds[i].testingSamples().size();

        if (!stream || total != ds.size())
            throw WUPException(INCOMPATIBLE_FILE_MESSAGE);
    }

    void
    createKFold(const Dataset &ds, const int numFolds)
    {
        _numFolds = numFolds;
        createFolds(numFolds);

        // Detecta a quantidade e quem pertence a qual classe
        //auto classes = ds.toUnorderedMap([](const Sample &sample){ return sample.id(); });

        std::map< int, ref_vector<const Sample> > classes;
        for (ulong i=0;i<ds.size();++i)
            classes[ds[i].subtarget()].push_back(&ds[i]);

        // Divide os grupos nos folds
        for (auto pair : classes)
        {
            const int length = pair.second.size();
            const uint * const mem = randperm(length);

            for (uint i=0;i<_numFolds;++i)
            {
                const int first = int(round(length * i / double(_numFolds)));
                const int last = int(round(length * (i+1) / double(_numFolds)));

                for (uint j=0; j<_numFolds; ++j)
                {
                    if (j == i)
                    {
                        for (int k=first;k<last;++k)
                            _folds[j].addTestingSample(pair.second[mem[k]]);
                    }
                    else
                    {
                        for (int k=first;k<last;++k)
                            _folds[j].addTrainingSample(pair.second[mem[k]]);
                    }
                }
            }

            delete [] mem;
        }
    }

public:

    KFold(const Dataset &ds, const int numFolds) : _ds(&ds),
        _numFolds(numFolds), _folds(NULL)
    {
        createKFold(ds, numFolds);
    }

    KFold(const Dataset &ds, std::istream &file_in) : _ds(&ds)
    {
        loadFromStream(ds, file_in);
    }

    KFold(const Dataset &ds, const std::string filePath) : _ds(&ds)
    {
        std::ifstream file_in( filePath );
        loadFromStream(ds, file_in);
        file_in.close();
    }

    KFold(const Dataset &ds, std::istream &file_in, const int numFolds) : _ds(&ds)
    {
        try
        {
            loadFromStream(ds, file_in);
        }
        catch (WUPException e)
        {
            print("Invalid KFold, creating new");
            createKFold(ds, numFolds);
        }
    }

    KFold(const Dataset &ds, const std::string filePath, const int numFolds) : _ds(&ds)
    {
        try
        {
            std::ifstream file_in(filePath);
            loadFromStream(ds, file_in);
            file_in.close();
        }
        catch (WUPException e)
        {
            print("Invalid KFold, creating new");
            createKFold(ds, numFolds);
            std::ofstream file_out(filePath);
            save(file_out);
        }
    }

    // This is used when the dataset already contains a kfold division
    KFold(const Dataset &ds) : _ds(&ds), _numFolds(0), _folds(NULL)
    {
        auto attributes = ds.attr();

        if (attributes.numCols() < 3)
            throw WUPException("This dataset does not contains an inner kfold");

        // Detecta o número de folds
        std::map<double, int> folds;
        for (uint i=0;i<attributes.numRows();++i)
        {
            const double key = attributes( i+1, 3 );
            if (folds.find(key) == folds.end())
                folds[key] = folds.size();
        }

        _numFolds = folds.size();
        createFolds(_numFolds);

        for (uint i=0;i<attributes.numRows();++i)
        {
            const uint f = folds[attributes( i+1, 3 )];
            for (uint j=0; j<_numFolds; ++j)
            {
                if (j == f)
                    _folds[j].addTestingSample(ds[i]);
                else
                    _folds[j].addTrainingSample(ds[i]);
            }
        }
    }

    ~KFold()
    {
        delete [] _folds;
    }

    const Fold * begin() const
    {
        return _folds;
    }

    const Fold * end() const
    {
        return &_folds[_numFolds];
    }

    const Fold * begin()
    {
        return _folds;
    }

    const Fold * end()
    {
        return &_folds[_numFolds];
    }

    int
    numFolds() const
    {
        return _numFolds;
    }

    const Dataset &
    dataset() const
    {
        return *_ds;
    }

    void
    save(std::ostream &file_out)
    {
        file_out << " " << -1;
        file_out << " " << _numFolds;

        for (uint f=0;f<_numFolds;++f)
        {
            file_out << " " << _folds[f].trainingSamples().size();
            file_out << " " << _folds[f].testingSamples().size();

            for (auto &sample : _folds[f].trainingSamples())
                file_out << " " << sample.id();

            for (auto &sample : _folds[f].testingSamples())
                file_out << " " << sample.id();
        }

        file_out << -1;
    }

};

}

#endif // KFOLD_HPP
