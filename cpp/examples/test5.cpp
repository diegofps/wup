#include <wup/wup.hpp>

using namespace wup;
using namespace wup::node;

#define ui std::initializer_list<uint>

WUP_STATICS;

void pattern1(StreamEncoder & encoder, Feature & f)
{
    encoder.clear();
    encoder.start(0);

    f[0]=+1.0; f[1]=+0.0; encoder.digest(f);
    f[0]=+1.0; f[1]=+1.0; encoder.digest(f);
    f[0]=+0.0; f[1]=+1.0; encoder.digest(f);
    f[0]=-1.0; f[1]=+1.0; encoder.digest(f);
    f[0]=-1.0; f[1]=+0.0; encoder.digest(f);
    f[0]=-1.0; f[1]=-1.0; encoder.digest(f);
    f[0]=+0.0; f[1]=-1.0; encoder.digest(f);
    f[0]=+1.0; f[1]=-1.0; encoder.digest(f);
    f[0]=+1.0; f[1]=+0.0; encoder.digest(f);

    encoder.finish();
}

void pattern2(StreamEncoder & encoder, Feature & f)
{
    encoder.clear();
    encoder.start(1);

    f[0]=-1.0; f[1]=-0.0; encoder.digest(f);
    f[0]=-1.0; f[1]=-1.0; encoder.digest(f);
    f[0]=-0.0; f[1]=-1.0; encoder.digest(f);
    f[0]=+1.0; f[1]=-1.0; encoder.digest(f);
    f[0]=+1.0; f[1]=-0.0; encoder.digest(f);
    f[0]=+1.0; f[1]=+1.0; encoder.digest(f);
    f[0]=-0.0; f[1]=+1.0; encoder.digest(f);
    f[0]=-1.0; f[1]=+1.0; encoder.digest(f);
    f[0]=-1.0; f[1]=-0.0; encoder.digest(f);

    encoder.finish();
}

int 
main(int argc, char * argv[])
{
    srand(time(NULL));

    // Model Parameters
    Params params(argc, argv);
    const int ramBits = 16;
    const int numKernels = 512;
    const double kernelActivation = 0.075;
    const int kernelBits = 4;

    // Create the model
    StreamEncoder encoder(2);
    encoder.add<Add>();
    encoder.add<ZScore>(true);
    encoder.add<Smooth4>(0.01);
    encoder.add<Direction>();
    encoder.add<ZScore>(ui{0l, 1l}, false);
    encoder.add<Tanh>(ui{0l, 1l});
    encoder.add<Replicate>(3);
    encoder.add<Show>("Sample", false, -1);
    encoder.add<node::KernelCanvas>(numKernels, kernelActivation, kernelBits).actAsPattern();
    Wisard w(encoder.patternSize(), ramBits, 2);

    // Display configuration
    print("numRams:", w.numRams());
    print("ramBits:", w.numRamBits());
    print("numInputBits:", w.numInputBits());

    // Train
    double tmp[2];
    Feature f(tmp, 2);

    pattern1(encoder, f);
    w.learn(encoder.pattern(), 0);
    print();

    pattern2(encoder, f);
    w.learn(encoder.pattern(), 1);
    print();

    // Predict
    pattern1(encoder, f);
    print(w.readBleaching(encoder.pattern()));
    print(w.getConfidence(), w.getExcitation(0), w.getExcitation(1));

    pattern2(encoder, f);
    print(w.readBleaching(encoder.pattern()));
    print(w.getConfidence(), w.getExcitation(0), w.getExcitation(1));

    return 0;
}
