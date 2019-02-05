#define LOGE ;
#define LOGD ;

#include <cstdlib>
#include <execinfo.h>
#include <wup/wup.hpp>
#include <wup/nodes/kernelwisard.hpp>

using namespace wup;
using namespace wup::node;

#define WISARD Wisard
#define ui std::initializer_list<uint>

void
evaluateFold(const Fold &fold,
             ConfusionMatrix &avgConfusionMatrix,
             node::StreamEncoder & encoder,
             const int ramBits)
{
    print(" --- Evaluating fold number ", fold.id(), " ---");
    print("Number of training samples: ", fold.trainingSamples().size());
    print("Number of testing samples: ", fold.testingSamples().size());
    ConfusionMatrix confusionMatrix(fold.dataset());

    {
        WISARD w(encoder.patternSize(), ramBits, fold.dataset().classes());

        for (auto &sample : fold.trainingSamples())
        {
            const int * const pattern = encoder.encode(sample);
            const int target = sample.target();
            w.learn(pattern, target);
//            break;
        }

//        iwriter writer(FILENAME);
//        w.exportTo(writer);
//    }

//    {
//        ireader reader(FILENAME);
//        WISARD w(reader);

        for (auto &sample : fold.testingSamples())
        {
            const int target = sample.target();
            const int * const pattern = encoder.encode(sample);
            confusionMatrix.add(w.readBleaching(pattern), target);
//            break;
        }
    }

    print("Confusion matrix:");
    std::cout << confusionMatrix << std::endl;

    avgConfusionMatrix.import(confusionMatrix);
}

void
evaluateFold2(const Fold &fold,
             ConfusionMatrix &avgConfusionMatrix,
             node::StreamEncoder & encoder,
             const int ramBits)
{
    print(" --- Evaluating fold number ", fold.id(), " ---");
    print("Number of training samples: ", fold.trainingSamples().size());
    print("Number of testing samples: ", fold.testingSamples().size());
    ConfusionMatrix confusionMatrix(fold.dataset());

    {
        wup::models::KernelWisard<EuclidianKernels> w(encoder.realPatternSize(), 2, 256, 0.0);

        print("Training...");
        for (auto &sample : fold.trainingSamples())
        {
            const double * const pattern = encoder.encodeReal(sample);
            const int target = sample.target();
            w.learn(pattern, target);
//            break;
        }

        print("Testing...");
        for (auto &sample : fold.testingSamples())
        {
            const int target = sample.target();
            const double * const pattern = encoder.encodeReal(sample);
            confusionMatrix.add(w.readBleaching(pattern), target);
//            break;
        }
    }

    print("Confusion matrix:");
    std::cout << confusionMatrix << std::endl;

    avgConfusionMatrix.import(confusionMatrix);
}

void
testLibras(Params & params)
{
    Dataset dataset("../../datasets/libras");
    KFold kfold(dataset, 10);

    const char * filename = "./chainbuilder.bin";

    {
        iwriter writer(filename);

        StreamEncoder *encoder = &(*new StreamEncoder(dataset.data().numCols()))
                        .add<ZScore>(true)
                        .add<Smooth4>(0.01)
                        .add<Direction>()
                        .add<ZScore>(ui{0l, 1l}, true)
                        .add<Tanh>(ui{0l, 1l})
                        .add<ShortMemory>(3)

//                        .add<Replicate>(6)
//                        .add<Shuffler>()
//                        .add<MultiKernelCanvas>(128, 3, 0.075, 20).actAsPattern()

                        .add<node::KernelCanvas>(2048, 0.075, 20).actAsPattern()

                        .exportTo(writer);

//        delete encoder;
//    }

//    {
//        ireader reader(filename);
//        auto encoder = new StreamEncoder(reader);

        ConfusionMatrix confusion(dataset);
        for (const Fold &fold : kfold)
            evaluateFold(fold, confusion, *encoder, 32);

        print("Avg Confusion Matrix:");
        std:: cout << confusion << std::endl;

        delete encoder;
    }
}

void
testLibras2(Params & params)
{
    Dataset dataset("../../datasets/libras");
    KFold kfold(dataset, 10);

    const char * filename = "./chainbuilder.bin";

    {
        iwriter writer(filename);

        StreamEncoder *encoder = &(*new StreamEncoder(dataset.data().numCols()))
                        .add<ZScore>(true)
                        .add<Smooth4>(0.01)
                        .add<Direction>()
                        .add<ZScore>(ui{0l, 1l}, true)
                        .add<Tanh>(ui{0l, 1l})
                        .add<ShortMemory>(3)
                        .add<Replicate>(6)
                        .add<Shuffler>().actAsPattern()

//                        .add<Replicate>(6)
//                        .add<Shuffler>()
//                        .add<MultiKernelCanvas>(128, 3, 0.075, 20).actAsPattern()

//                        .add<node::KernelCanvas>(2048, 0.075, 20).actAsPattern()

                        .exportTo(writer);

//        delete encoder;
//    }

//    {
//        ireader reader(filename);
//        auto encoder = new StreamEncoder(reader);

        ConfusionMatrix confusion(dataset);
        for (const Fold &fold : kfold)
            evaluateFold2(fold, confusion, *encoder, 32);

        print("Avg Confusion Matrix:");
        std:: cout << confusion << std::endl;

        delete encoder;
    }
}

void
testUji2(Params & params)
{
    Dataset dataset("./data/uji2");
    KFold kfold(dataset, "./data/kfold_uji2", 10, true);
    print(kfold);
    for (int i=0;i<10;++i)
        print(" ", kfold.fold(0).trainingSamples()[i].id());
    //getchar();

    const char * filename = "./chainbuilder.bin";

    {
        iwriter writer(filename);

        StreamEncoder *encoder = &(*new StreamEncoder(dataset.data().numCols()))
                        .add<ZScore>(true)
                        .add<Smooth4>(0.01)
                        .add<Direction>()
                        .add<ZScore>(ui{0l, 1l}, false)
                        .add<Tanh>(ui{0l, 1l})
                        .add<Replicate>(3)
                //.add<Show>("Sample", false, 5085)
                        .add<MultiKernelCanvas>(128, 2, 0.05, 20).actAsPattern()
                        //.add<MultiKernelCanvas>(20, 128, 8, 0.07, 20).actAsPattern()
                        //.add<node::KernelCanvas>(2048, 0.01, 16).actAsPattern()
                        .exportTo(writer);

//        delete encoder;
//    }

//    {
//        ireader reader(filename);
//        auto encoder = new StreamEncoder(reader);

        ConfusionMatrix confusion(dataset);
        for (const Fold &fold : kfold)
            evaluateFold(fold, confusion, *encoder, 32);

        print("Avg Confusion Matrix:");
        std::cout << confusion << std::endl;

        delete encoder;
    }
}

void
testArabic(Params & params)
{
    Dataset dataset("./data/arabic");
    KFold kfold(dataset, "./data/kfold_arabic", 10, true);
    print(kfold);
    for (int i=0;i<10;++i)
        print(" ", kfold.fold(0).trainingSamples()[i].id());
    //getchar();

    const char * filename = "./chainbuilder.bin";

    {
        iwriter writer(filename);

        StreamEncoder *encoder = &(*new StreamEncoder(dataset.data().numCols()))
                        .add<ZScore>(true)
                        .add<Smooth4>(0.01)
                        .add<Direction>()
                        .add<ZScore>(ui{0l, 1l}, false)
                        .add<Tanh>(ui{0l, 1l})
                        .add<Replicate>(3)
                //.add<Show>("Sample", false, 5085)
                        //.add<MultiKernelCanvas>(20, 128, 2, 0.05, 20).actAsPattern()
                        //.add<MultiKernelCanvas>(20, 128, 8, 0.07, 20).actAsPattern()
                        .add<node::KernelCanvas>(2048, 0.01, 16).actAsPattern()
                        .exportTo(writer);

//        delete encoder;
//    }

//    {
//        ireader reader(filename);
//        auto encoder = new StreamEncoder(reader);

        ConfusionMatrix confusion(dataset);
        for (const Fold &fold : kfold)
            evaluateFold(fold, confusion, *encoder, 32);

        print("Avg Confusion Matrix:");
        std::cout << confusion << std::endl;

        delete encoder;
    }
}

int
main(int argc, const char **argv)
{
    srand(time(NULL));

    Params params(argc, argv);

    //testLibras(params);
    testLibras2(params);
    //testUji2(params);
    //testArabic(params);

    return 0;
}
