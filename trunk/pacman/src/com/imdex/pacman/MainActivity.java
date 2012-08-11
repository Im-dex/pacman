package com.imdex.pacman;

import android.os.Bundle;
import android.app.Activity;

public class MainActivity extends Activity {

	private SurfaceView mView;
	private ErrorReporter mErrorReporter;
	private ScreenInformation mScreenInformation;
	private boolean mNormalStart = true;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        InputListener inputListener = new InputListener();
        mErrorReporter = new ErrorReporter(this);
    	try {
			mScreenInformation = new ScreenInformation(this.getResources().getConfiguration(), this.getWindowManager());
		} catch (Exception e) {
			mNormalStart = false;
			mErrorReporter.terminateApplication(e.getMessage());
			return;
		}
    	
        NativeLib.setContext(getApplicationContext());
        NativeLib.setErrorReporter(mErrorReporter);
        
        mView = new SurfaceView(this, mErrorReporter);
        mView.setOnTouchListener(inputListener);
        
        setContentView(mView);
    }
    
    @Override
    public void onStart() {
    	super.onStart();
    	if (mNormalStart)
    		NativeLib.init(mScreenInformation.screenSize, mScreenInformation.screenDensity);
    }
    
    @Override
    public void onStop() {
    	super.onStop();
    	if (mNormalStart)
    		NativeLib.deinit();
    }
    
    @Override
    public void onResume() {
    	super.onResume();
    	if (mNormalStart)
    		mView.onResume();
    }
    
    @Override
    public void onPause() {
    	super.onPause();
    	if (mNormalStart)
    		mView.onPause();
    }

}
