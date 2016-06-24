package com.github.diegofps.android.andwup.examples;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		View btTest1 = findViewById(R.id.btTest1);
		btTest1.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				new Test1().run();
			}
		});
		
		
	}
	
}
