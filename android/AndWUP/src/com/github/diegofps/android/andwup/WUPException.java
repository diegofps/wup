package com.github.diegofps.android.andwup;

public class WUPException extends RuntimeException {

	/**
	 * 
	 */
	private static final long serialVersionUID = -9204017529411876813L;

	public WUPException() {
		super();
	}

	public WUPException(String detailMessage, Throwable throwable) {
		super(detailMessage, throwable);
	}

	public WUPException(String detailMessage) {
		super(detailMessage);
	}

	public WUPException(Throwable throwable) {
		super(throwable);
	}

}
