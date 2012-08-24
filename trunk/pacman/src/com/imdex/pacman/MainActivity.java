package com.imdex.pacman;

import android.os.Bundle;
import android.app.Activity;

public class MainActivity extends Activity {

	private SurfaceView mView;
	private ErrorReporter mErrorReporter;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        InputListener inputListener = new InputListener();
        mErrorReporter = new ErrorReporter(this);
    	
        NativeLib.setContext(getApplicationContext());
        NativeLib.setErrorReporter(mErrorReporter);
        
        mView = new SurfaceView(this, mErrorReporter);
        mView.setOnTouchListener(inputListener);
        
        setContentView(mView);
    }
    
    @Override
    public void onStart() {
    	super.onStart();
    	NativeLib.init();
    }
    
    @Override
    public void onStop() {
    	super.onStop();
    	NativeLib.deinit();
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
