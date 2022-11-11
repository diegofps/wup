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

WUP_STATICS;

void
evaluateFold(const Fold &fold,
             ConfusionMatrix &avgConfusionMatrix,
             node::StreamEncoder & encoder,
             const int ramBits)
{
    // print(" --- Evaluating fold number ", fold.id(), " ---");
    // print("Number of training samples: ", fold.trainingSamples().size());
    // print("Number of testing samples: ", fold.testingSamples().size());
    ConfusionMatrix confusionMatrix(fold.dataset().classes());

    {
        WISARD w(encoder.patternSize(), ramBits, fold.dataset().classes());

        for (auto &sample : fold.trainingSamples())
        {
            const int * const pattern = encoder.encode(sample);
            const int target = sample.target();
            w.learn(pattern, target);
//            break;
        }

//        IntFileWriter writer(FILENAME);
//        w.exportTo(writer);
//    }

//    {
//        IntFileReader reader(FILENAME);
//        WISARD w(reader);

        for (auto &sample : fold.testingSamples())
        {
            const int target = sample.target();
            const int * const pattern = encoder.encode(sample);
            confusionMatrix.add(w.readBleaching(pattern), target);
        }
    }

    // confusionMatrix.update();
    // print("Confusion matrix:");
    // std::cout << confusionMatrix << std::endl;

    avgConfusionMatrix.import(confusionMatrix);
}

void
testLibras(Params & params, double activation)
{
    Dataset dataset("../../datasets/libras");
    KFold kfold(dataset, 10);

    const char * filename = "./chainbuilder.bin";

    {
        IntFileWriter writer(filename);

        StreamEncoder *encoder = &(*new StreamEncoder(dataset.data().numCols()))
                        .add<ZScore>(true)
                        .add<Smooth4>(0.01)
                        .add<Direction>()
                        .add<ZScore>(ui{0l, 1l}, true)
                        .add<Tanh>(ui{0l, 1l})
                        .add<ShortMemory>(3)

                        .add<node::KernelCanvas>(2048, activation, 20).actAsPattern()

                        .exportTo(writer);

//        delete encoder;
//    }

//    {
//        IntFileReader reader(filename);
//        auto encoder = new StreamEncoder(reader);

        ConfusionMatrix confusion(dataset.classes());
        for (const Fold &fold : kfold)
            evaluateFold(fold, confusion, *encoder, 16);

        confusion.update();
        // print("Avg Confusion Matrix:");
        // std:: cout << confusion << std::endl;
        print("libras", confusion.accuracy());

        delete encoder;
    }
}

void
testUji2(Params & params, double activation)
{
    Dataset dataset("./data/uji2");
    KFold kfold(dataset, "./data/kfold_uji2", 10, true);

    const char * filename = "./chainbuilder.bin";

    {
        IntFileWriter writer(filename);

        StreamEncoder *encoder = &(*new StreamEncoder(dataset.data().numCols()))
                        .add<ZScore>(true)
                        .add<Smooth4>(0.01)
                        .add<Direction>()
                        .add<ZScore>(ui{0l, 1l}, false)
                        .add<Tanh>(ui{0l, 1l})
                        .add<Replicate>(3)

                        .add<node::KernelCanvas>(2048, activation, 16).actAsPattern()

                        .exportTo(writer);

//        delete encoder;
//    }

//    {
//        IntFileReader reader(filename);
//        auto encoder = new StreamEncoder(reader);

        ConfusionMatrix confusion(dataset.classes());

        for (const Fold &fold : kfold)
            evaluateFold(fold, confusion, *encoder, 32);

        confusion.update();
        // print("Avg Confusion Matrix:");
        // std::cout << confusion << std::endl;
        print("uji2", confusion.accuracy());

        delete encoder;
    }
}

void
testArabic(Params & params, double activation, double smooth, int memory)
{
    Dataset dataset("./data/arabic");
    KFold kfold(dataset, "./data/kfold_arabic", 10, true);
    const char * filename = "./chainbuilder.bin";

    {
        IntFileWriter writer(filename);

        StreamEncoder *encoder = &(*new StreamEncoder(dataset.data().numCols()))
                        .add<ZScore>(true)
                        .add<Smooth4>(smooth) // 0.01
                        .add<Direction>()
                        .add<ZScore>(ui{0l, 1l}, false)
                        .add<Tanh>(ui{0l, 1l})
                        .add<ShortMemory>(memory) // 3
                
                        .add<node::KernelCanvas>(2048, activation, 32).actAsPattern()

                        .exportTo(writer);

//        delete encoder;
//    }

//    {
//        ireader reader(filename);
//        auto encoder = new StreamEncoder(reader);

        ConfusionMatrix confusion(dataset.classes());
        for (const Fold &fold : kfold)
            evaluateFold(fold, confusion, *encoder, 32);

        confusion.update();
        // print("Avg Confusion Matrix:");
        // std::cout << confusion << std::endl;
        print("arabic: act =", activation, " smooth =", smooth, "memory =", memory, "acc =", confusion.accuracy());

        delete encoder;
    }
}

int
main(int argc, const char **argv)
{
    srand(time(NULL));

    Params params(argc, argv);

    // testArabic(params, 8);
    // testArabic(params, 16);

    std::vector<int> activations = {8,16};
    std::vector<int> memories = {1,2,3,4,5};

    for (int act : activations)
        for(double smooth=0;smooth<0.05;smooth+=0.005)
            for (int mem : memories)
                testArabic(params, act, smooth, mem);

    // for (int i=1;i!=17;++i)
    // {
    //     print("activation =", i);
    //     // testLibras(params, i);
    //     // testUji2(params, i);
    //     testArabic(params, i);
    //     print();
    // }

    return 0;
}
