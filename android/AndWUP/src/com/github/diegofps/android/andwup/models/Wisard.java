package com.github.diegofps.android.andwup.models;

import com.github.diegofps.android.andwup.NativeWrapper;
import com.github.diegofps.android.andwup.WUPException;

public class Wisard extends NativeWrapper {

	public Wisard(int inputBits, int ramBits) {
		mNative = native_create(inputBits, ramBits);
	}

	public Wisard(int inputBits, int ramBits, int classes) {
		mNative = native_create(inputBits, ramBits, classes);
	}
	
	public Wisard(String filename) {
		mNative = native_create(filename);
	}
	
	public void close() {
		checkClosed();
		native_close(mNative);
		mNative = 0;
	}
	
	public void exportTo(String filename) {
		checkClosed();
		native_exportTo(mNative, filename);
	}
	
	public int classes() {
		checkClosed();
		return native_classes(mNative);
	}
	
	public int ramBits() {
		checkClosed();
		return native_ramBits(mNative);
	}
	
	public int inputBits() {
		checkClosed();
		return native_inputBits(mNative);
	}
	
	public void learn(int[] pattern, int target) {
		checkClosed();
		
		if (pattern == null || pattern.length==0)
			throw new WUPException("Invallid pattern");
		
		native_learn(mNative, pattern, target);
	}
	
	public void forget(int target) {
		checkClosed();
		native_forget(mNative, target);
	}

	public int readCounts(int[] pattern) {
		checkClosed();

		if (pattern == null || pattern.length==0)
			throw new WUPException("Invallid pattern");
		
		return native_readCounts(mNative, pattern);
	}

	public int readBleaching(int[] pattern) {
		checkClosed();
		if (pattern == null || pattern.length==0)
			throw new WUPException("Invallid pattern");
		
		return native_readBleaching(mNative, pattern);
	}

	public int readBleaching(int[] pattern, int step, float minConfidence) {
		checkClosed();

		if (pattern == null || pattern.length==0)
			throw new WUPException("Invallid pattern");
		
		return native_readBleaching(mNative, pattern, step, minConfidence);
	}

	public int readBinary(int[] pattern) {
		checkClosed();

		if (pattern == null || pattern.length==0)
			throw new WUPException("Invallid pattern");
		
		return native_readBinary(mNative, pattern);
	}

	public float getConfidence() {
		checkClosed();
		return native_getConfidence(mNative);
	}

	public float getExcitation(int target) {
		checkClosed();
		return native_getExcitation(mNative, target);
	}

	public int getFirstBestPrediction() {
		checkClosed();
		return native_getFirstBestPrediction(mNative);
	}

	public int getSecondBestPrediction() {
		checkClosed();
		return native_getSecondBestPrediction(mNative);
	}

	public int getThirdBestPrediction() {
		checkClosed();
		return native_getThirdBestPrediction(mNative);
	}

	private void checkClosed() {
		if (mNative == 0)
			throw new WUPException("Already closed");
	}
	
	private static native long native_create(int inputBits, int ramBits);
	
	private static native long native_create(int inputBits, int ramBits, int classes);
	
	private static native long native_create(String filename);
	
	private static native void native_close(long self);
	
	private static native void native_exportTo(long self, String filename);
	
	private static native int native_classes(long self);
	
	private static native int native_ramBits(long self);
	
	private static native int native_inputBits(long self);
	
	private static native void native_learn(long self, int[] pattern, int target);
	
	private static native void native_forget(long self, int target);

	private static native int native_readCounts(long self, int[] pattern);

	private static native int native_readBleaching(long self, int[] pattern);

	private static native int native_readBleaching(long self, int[] pattern, int step, float minConfidence);

	private static native int native_readBinary(long self, int[] pattern);
	
	private static native float native_getConfidence(long self);
	
	private static native float native_getExcitation(long self, int target);
	
	private static native int native_getFirstBestPrediction(long self);
	
	private static native int native_getSecondBestPrediction(long self);
	
	private static native int native_getThirdBestPrediction(long self);
	
}


