package com.github.diegofps.android.andwup;

public class NativeWrapper {

	static {
		System.loadLibrary("awup");
	}
	
	protected long mNative;
	
	public long ni() {
		return mNative;
	}
	
}
