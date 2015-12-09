#include <wup/wup.hpp>

using namespace wup;

void
fold_eval(Fold &fold, void * data)
{
    node::Root          root(2);
    node::Smooth        smooth(root, 0.3);
    node::Direction     direction(smooth);
    node::ZScore        zscore(direction);
    node::Tanh          tanh(zscore);
    node::Replicate     replicate(tanh, 3);
    node::KernelCanvas  kernelCanvas(replicate, 2048, 0.075, 20);
    
    root.addEmitter(kernelCanvas);
    Wisard w(root.outputLength(), 16, 15);
    
    for (auto &sample : fold.trainset)
        w.learn(root.digest(sample), sample.target());
    
    /*
    for (auto &sample : fold.trainset) {
        root.start();
        for (auto &feature : sample)
            root.digest(feature);
        root.finish();
        w.learn(root.buildBinaryPattern(), sample.target());
    }
    */
    
    Confusion &confusion = *((Confusion*)data);
    for (auto &sample : fold.testset)
        confusion.add(w.digest(root.read(sample)), sample.target());

    print("Results for fold number ", fold.id());
    print(confusion);
}

int
main() {
    Dataset dataset("libras");
    KFold kfold(dataset, "kfold.bin");
    Confusion confusion;
    
    for (auto &fold : kfold)
        eval_fold(fold, &confusion);

    print confusion;
    
    return 0;
}

