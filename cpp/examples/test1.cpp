#include <iostream>
#include <wup/wup.hpp>

using namespace wup;

const int TRAIN_ITERS = 20;
const int TEST_ITERS = 10;
const int LENGTH = 512;

const int RAM_BITS = 16;
const int CLASSES = 500;

const char * FILENAME = "wisard.bin";

int *
noisify(const int * const src, int * const dst, 
    const int length, const double noise)
{
    memcpy(dst, src, sizeof(int) * length);
    addNoise(dst, length, noise);
    return dst;
}

void
test(const double noise)
{
    int ** patterns = new int*[CLASSES];
    int *pattern = new int[LENGTH];
    int misses = 0;
    int hits = 0;
    
    Clock trainTime, testTime, exportTime, importTime;
    
    // Creates a sample pattern for each class
    for (int i=0;i<CLASSES;++i)
        patterns[i] = randomPattern(LENGTH);

    // Performs creation, training and exportTo
    {
        // Creates a Wisard Network with two discriminators
        Wisard w(LENGTH, RAM_BITS, CLASSES);
        
        // Perform training
        trainTime.start();
        for (int i=0;i<CLASSES;++i) {
            for (int j=0;j<TRAIN_ITERS;++j) {
                noisify( patterns[i], pattern, LENGTH, noise );
                w.learn( pattern, i );
            }
        }
        trainTime.stop();
        
        exportTime.start();
        sbwriter<int> writer(FILENAME);
        w.exportTo(writer);
        exportTime.stop();
    }
    
    // Perform import, testing
    {
        importTime.start();
        sbreader<int> reader(FILENAME);
        Wisard w2(reader);
        importTime.stop();
        
        testTime.start();
        for (int i=0;i<CLASSES;++i) {
            for (int j=0;j<TEST_ITERS;++j) {
                noisify( patterns[i], pattern, LENGTH, noise );
                const int predicted = w2.readBleaching( pattern );
                
                if (predicted == i) ++hits;
                else ++misses;
            }
        }
        testTime.stop();
    }
    
    // Clean memory
    for (int i=0;i<CLASSES;++i)
        delete [] patterns[i];
    
    delete [] patterns;
    delete [] pattern;
    
    // Print results
    print( GREEN,
         "Noise = ", noise * 100, "%",
         ", Accuracy = ", hits / double(hits + misses), 
         ", Train time = ", trainTime.ellapsed_seconds(), "s",
         ", Test time = ", testTime.ellapsed_seconds(), "s", 
         ", ExportTime = ", exportTime.ellapsed_seconds(), "s",
         ", ImportTime = ", importTime.ellapsed_seconds(), "s", 
         NORMAL );
}

int 
main(int argc, char **argv)
{
    srand(time(NULL));
    
    Clock total;
    for (auto noise : xrange(0, 101, 5))
        test( noise / 100.0 );
    total.stop();
    
    print("Tempo total:", total.ellapsed_seconds(), "s");   
    
    return 0;
}

