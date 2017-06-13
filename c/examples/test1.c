#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <wup/wup.h>

const int TRAIN_ITERS = 1000;
const int TEST_ITERS = 1000;
const int LENGTH = 128;

const int RAM_BITS = 16;
const int CLASSES = 10;

const char * FILENAME = "wisard.bin";

int *
randomPattern(const int length)
{
    int * array = (int*) malloc( sizeof(int) * length );
    for (int i=0;i<length;++i)
        array[i] = rand() % 2;
    return array;
}

void
addNoise(int * const pattern, const int length, 
    const double noise)
{
    for (int i=0;i<length;++i)
        if (rand() / (double) RAND_MAX < noise)
            pattern[i] = rand() % 2;
}

int *
noisify(const int * const src, int * const dst, 
    const int length, const double noise)
{
    memcpy( dst, src, sizeof(int) * length );
    addNoise( dst, length, noise );
    return dst;
}

void
test(const double noise)
{
    int ** patterns = (int**) malloc(sizeof(int*) * CLASSES);
    int *pattern = (int*) malloc(sizeof(int) * LENGTH);
    int misses = 0;
    int hits = 0;
    
    // Creates a sample pattern for each class
    for (int i=0;i<CLASSES;++i)
        patterns[i] = randomPattern( LENGTH );
    
    {
        // Creates a Wisard Network with two discriminators
        CWisard w;
        
        wisard_create( &w, LENGTH, RAM_BITS, CLASSES );
        
        // Perform training
        for (int i=0;i<CLASSES;++i) {
            for (int j=0;j<TRAIN_ITERS;++j) {
                noisify( patterns[i], pattern, LENGTH, noise );
                wisard_learn( &w, pattern, i );
            }
        }
        
        wisard_exportTo( &w, FILENAME );
        
        // Removes this NN from the memory
        wisard_destroy( &w );
    }
    
    {
        CWisard w2;
        wisard_importFrom( &w2, FILENAME );
     
        // Perform testing
        for (int i=0;i<CLASSES;++i) {
            for (int j=0;j<TEST_ITERS;++j) {
                noisify( patterns[i], pattern, LENGTH, noise );
                const int predicted = wisard_readBleaching( &w2, pattern );
                
                if (predicted == i) ++hits;
                else ++misses;
            }
        }
        
        // Removes this NN from the memory
        wisard_destroy( &w2 );
    }
    
    // Clean memory
    for (int i=0;i<CLASSES;++i)
        free( patterns[i] );
    
    free( patterns );
    free( pattern );
    
    // Print results
    printf( "Noise = %d, Accuracy = %f\n", 
            (int)(noise * 100), 
            hits / (double)(hits + misses) );
}

int 
main(int argc, char **argv)
{
    srand( time( NULL ) );
    
    for (int noise=0;noise<101;noise+=5)
        test( noise / 100.0 );
    
    return 0;
}

