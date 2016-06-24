package com.github.diegofps.android.andwup.nodes;

import java.util.List;

import com.github.diegofps.android.andwup.NativeWrapper;

public class Root extends NativeWrapper implements Node {

	public Root(int size) {
		
	}
	
	@Override
	public void start() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void finish() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void digest(double[] feature) {
		// TODO Auto-generated method stub
		
	}

	public int[] digest(List<double[]> features) {
		return null;
	}
	
	public void addEmitter(Node node) {
		
	}
	
	public int binaryOutputLength() {
		return 0;
	}
	
	public int[] binaryOutput() {
		return null;
	}
}
