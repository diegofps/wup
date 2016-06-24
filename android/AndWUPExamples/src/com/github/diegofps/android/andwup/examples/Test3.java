package com.github.diegofps.android.andwup.examples;

import com.github.diegofps.android.andwup.models.Wisard;
import com.github.diegofps.android.andwup.nodes.Direction;
import com.github.diegofps.android.andwup.nodes.KernelCanvas;
import com.github.diegofps.android.andwup.nodes.Replicate;
import com.github.diegofps.android.andwup.nodes.Root;
import com.github.diegofps.android.andwup.nodes.Rotate;
import com.github.diegofps.android.andwup.nodes.Smooth;
import com.github.diegofps.android.andwup.nodes.Tanh;
import com.github.diegofps.android.andwup.nodes.ZScore;

public class Test3 extends Thread {

	@Override
	public void run() {
		int[] columns = new int[] {0, 1};
		
	    Root         root         = new Root(2);
	    ZScore       zscore1      = new ZScore(root, true);
	    Smooth       smooth       = new Smooth(zscore1, 0.01);
	    Direction    direction    = new Direction(smooth);
	    ZScore       zscore2      = new ZScore(direction, columns, false);
	    Rotate       rotate       = new Rotate(zscore2);
	    Tanh         tanh         = new Tanh(rotate, columns);
	    Replicate    replicate    = new Replicate(tanh, 3);
	    KernelCanvas kernelCanvas = new KernelCanvas(replicate, 1024, 0.01, 16);

	    root.addEmitter(kernelCanvas);

	    Wisard w = new Wisard(root.binaryOutputLength(), 32, 2);

	    
	    
	}
	
}
