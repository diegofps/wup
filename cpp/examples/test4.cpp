
#define LOGE ;
#define LOGD ;


#include <cstdlib>
#include <execinfo.h>
#include <wup/wup.hpp>

using namespace wup;
using namespace wup::node;

const int BITS = 16;

const char * FILENAME = "temp.bin";

#define WISARD Wisard

void
evaluateFold(const Fold &fold, ConfusionMatrix &avgConfusionMatrix, node::StreamEncoder & encoder)
{
    print(" --- Evaluating fold number ", fold.id(), " ---");
    print("Number of training samples: ", fold.trainingSamples().size());
    print("Number of testing samples: ", fold.testingSamples().size());
    ConfusionMatrix confusionMatrix(fold.dataset());

    {
        WISARD w(encoder.patternSize(), BITS, fold.dataset().classes());

        for (auto &sample : fold.trainingSamples())
        {
            const int * const pattern = encoder.encode(sample);
            const int target = sample.target();
            w.learn(pattern, target);
//            break;
        }
        sbwriter<int> writer(FILENAME);
        w.exportTo(writer);
    }

    {
        sbreader<int> reader(FILENAME);
        WISARD w(reader);

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

int
main(int argc, const char **argv) {
    srand(time(NULL));

    Params params(argc, argv);
    Dataset dataset("../../datasets/libras");
    KFold kfold(dataset, 10);

    const char * filename = "./chainbuilder.bin";

    {
        sbwriter<double> writer(filename);
        StreamEncoder *preencoder = &(*new StreamEncoder(dataset.data().numCols()))
                        .add<ZScore>(true)
                        .add<Smooth4>(0.01)
                        .add<Direction>()
                        .add<ZScore>(std::initializer_list<uint>{0l, 1l}, true)
                        .add<Tanh>(std::initializer_list<uint>{0l, 1l})
                        .add<Replicate>(3)
                        .add<node::KernelCanvas>(1024, 0.07, 20, 2).actAsPattern()
                        .exportTo(writer);

        delete preencoder;
    }

    {
        sbreader<double> reader(filename);
        auto encoder = new StreamEncoder(reader);

        ConfusionMatrix confusion(dataset);
        for (const Fold &fold : kfold)
            evaluateFold(fold, confusion, *encoder);

        print("Avg Confusion Matrix:");
        std::cout << confusion << std::endl;

        delete encoder;
    }

    return 0;
}
