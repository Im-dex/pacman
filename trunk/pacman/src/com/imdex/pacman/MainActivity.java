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
    public void onStart() {
    	super.onStart();
    	NativeLib.start();
    }
    
    @Override
    public void onStop() {
    	super.onStop();
    	NativeLib.stop();
    }
    
    @Override
    public void onResume() {
    	super.onResume();
		NativeLib.resume();
    	mView.onResume();
    }
    
    @Override
    public void onPause() {
    	super.onPause();
		NativeLib.pause();
    	mView.onPause();
    }

}
