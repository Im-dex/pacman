package com.imdex.pacman;

import android.os.Bundle;
import android.app.Activity;

public class MainActivity extends Activity {

	private SurfaceView mView;
	private Reporter mReporter;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        InputListener inputListener = new InputListener();
        mReporter = new Reporter(this);
    	
        NativeLib.setContext(getApplicationContext());
        NativeLib.setReporter(mReporter);
        
        mView = new SurfaceView(this, mReporter);
        mView.setOnTouchListener(inputListener);
        
        setContentView(mView);
    }
        
    @Override
    public void onResume() {
    	super.onResume();
    	mView.onResume();
    }
    
    @Override
    public void onPause() {
    	super.onPause();
    	mView.onPause();
    }

}
