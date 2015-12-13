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
    node::KernelCanvas kernelCanvas(&replicate, 1024, 0.01, 16);

    root.addEmitter(&kernelCanvas);

    Wisard w(root.binaryOutputLength(), 32, dataset.classes());

    const int length = dataset.size();
    const int threshold = length * 0.9;
    const int * const indexes = randperm(length);

    print("Training...");
//    for (int degrees=-5;degrees<=+5;degrees+=5) {
//    	print("Degrees:", degrees);
//    	rotate.angle(degrees);

    	Clock trainTime;
		for (int i=0;i<threshold;++i) {
			if (i%10==0) printn("\r",i,"/",threshold);
			Sample & sample = dataset[indexes[i]];
			w.learn(root.digest(sample), sample.target());
		}
		trainTime.stop();
		print(", UnitTrainTime(ms): ", trainTime.ellapsed() / double(threshold) / 1000.0);
//    }

    print();

    for (int c=0;c<dataset.classes();++c) {
		print("Testing...");
		int hits=0, misses=0;
		Clock testTime;
		for (int i=threshold;i<length;++i) {
			if (i%10==0) printn("\r",i-threshold,"/",length-threshold);
			Sample & sample = dataset[indexes[i]];
			if (w.readBleaching(root.digest(sample)) == sample.target())
				++hits;
			else
				++misses;
		}
		testTime.stop();
		print(", UnitTestTime(ms): ", testTime.ellapsed() / double(length-threshold) / 1000.0);

		print("Accuracy: ", hits / double(hits + misses));

		Clock forgetTime;
		w.forget(c);
		forgetTime.stop();
		print("TimeToForget(ms): ", forgetTime.ellapsed() / 1000.0);
    }

    return 0;
}

