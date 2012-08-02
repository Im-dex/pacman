package com.imdex.pacman;

import android.os.Bundle;
import android.app.Activity;

public class MainActivity extends Activity {

	private SurfaceView mView;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        NativeLib.setContext(getApplicationContext());
        InputListener inputListener = new InputListener();
        mView = new SurfaceView(getApplicationContext());
        mView.setOnTouchListener(inputListener);
        setContentView(mView);
    }
    
    @Override
    public void onPause() {
    	super.onPause();
    	mView.onPause();
    }
    
    @Override
    public void onResume() {
    	super.onResume();
    	mView.onResume();
    }
}
