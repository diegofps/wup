#include <wup/wup.hpp>

using namespace wup;

int
main() {
	srand(time(NULL));

	Dataset dataset("../../datasets/uji2");
    
    node::Root         root(dataset.numFeatures());
    node::ZScore       zscore1(&root, true);
    node::Smooth       smooth(&zscore1, 0.01);
    node::Direction    direction(&smooth);
    node::ZScore       zscore2(&direction, seq<int>(0,1), false);
    node::Rotate       rotate(&zscore2);
    node::Tanh         tanh(&rotate, seq<int>(0,1));
    node::Replicate    replicate(&tanh, 3);
    node::KernelCanvas kernelCanvas(&replicate, 2048, 0.01, 16);

    root.addEmitter(&kernelCanvas);

    Wisard w(root.binaryOutputLength(), 32, dataset.classes());

    const int length = dataset.size();
    const int threshold = length * 0.9;
    const int * const indexes = randperm(length);

    print("Training...");
    for (int degrees=-5;degrees<=+5;degrees+=5) {
    	print("Degrees:", degrees);
    	rotate.angle(degrees);

		for (int i=0;i<threshold;++i) {
			Sample & sample = dataset[indexes[i]];
			w.learn(root.digest(sample), sample.target());
		}
    }

    print();

    print("Testing...");
    int hits=0, misses=0;
    for (int i=threshold;i<length;++i) {
    	Sample & sample = dataset[indexes[i]];
		if (w.readBleaching(root.digest(sample)) == sample.target())
			++hits;
		else
			++misses;
    }

    print("Accuracy: ", hits / double(hits + misses));

    return 0;
}

