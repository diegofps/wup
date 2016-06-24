package com.github.diegofps.android.andwup.examples;

import java.io.File;
import java.util.Random;

import android.os.Environment;
import android.util.Log;

import com.github.diegofps.android.andwup.models.Wisard;

public class Test1 extends Thread {
	
	private final int TRAIN_ITERS = 1000;
	private final int TEST_ITERS = 1000;
	private final int LENGTH = 128;
	
	private final int RAM_BITS = 16;
	private final int CLASSES = 10;
	
	private final File SAVE_FILE = new File(Environment.getExternalStorageDirectory(), "wisard.bin");
	
	private Random random = new Random();
	
	private int[] noisify(int[] src, int[] dst, 
	    int length, double noise)
	{
		for (int i=0;i<src.length;++i) 
			if (random.nextDouble()<noise)
				dst[i] = random.nextInt(2);
			else
				dst[i] = src[i];
	    return dst;
	}
	
	private void test(double noise)
	{
	    int[][] patterns = new int[CLASSES][LENGTH];
	    int[] pattern = new int[LENGTH];
	    int misses = 0;
	    int hits = 0;
	    
	    // Creates a sample pattern for each class
	    for (int i=0;i<CLASSES;++i)
	    	for (int j=0;j<LENGTH;++j)
	    		patterns[i][j] = random.nextInt(2);
	
	    // Performs creation, training and exportTo
        // Creates a Wisard Network with two discriminators
        Wisard w = new Wisard(LENGTH, RAM_BITS, CLASSES);
        
        // Perform training
        for (int i=0;i<CLASSES;++i) {
            for (int j=0;j<TRAIN_ITERS;++j) {
                noisify( patterns[i], pattern, LENGTH, noise );
                w.learn( pattern, i );
            }
        }
        
//        Log.d("Saving to", SAVE_FILE.getAbsolutePath());
        w.exportTo(SAVE_FILE.getAbsolutePath());
	    
	    // Perform import, testing
        Wisard w2 = new Wisard(SAVE_FILE.getAbsolutePath());
        
        for (int i=0;i<CLASSES;++i) {
            for (int j=0;j<TEST_ITERS;++j) {
                noisify( patterns[i], pattern, LENGTH, noise );
                int predicted = w2.readBleaching( pattern );
                
                if (predicted == i) ++hits;
                else ++misses;
            }
        }
	    
	    // Print results
	    System.out.println(
	         "Noise = " + noise * 100 + "%" +
	         ", Accuracy = " + hits / (double)(hits + misses) );
	}

	@Override
	public void run() {
		long start = System.currentTimeMillis();
	    for (int noise=0;noise<=100;noise += 5) {
	        test( noise / 100.0 );
	    }
	    System.out.println("Total time: " + (System.currentTimeMillis() - start)/1000.0);
	}
	
}
