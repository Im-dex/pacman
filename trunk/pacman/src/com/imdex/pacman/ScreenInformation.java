package com.imdex.pacman;

import android.content.res.Configuration;
import android.util.DisplayMetrics;
import android.view.WindowManager;

public class ScreenInformation {

	// values that available since API level 9
	public static final int  SCREENLAYOUT_SIZE_XLARGE = 4;
	public static final int  DENSITY_XHIGH = 320;
	
	public final int screenSize;
	public final int screenDensity;
	
	public ScreenInformation(Configuration configuration, WindowManager windowManager) throws Exception {
		screenSize = GetSize(configuration);
		screenDensity = GetDensity(windowManager);
	}

	private int GetSize(Configuration configuration) throws Exception {
		int screenLayout = configuration.screenLayout;
		if ((screenLayout & Configuration.SCREENLAYOUT_SIZE_SMALL) == Configuration.SCREENLAYOUT_SIZE_SMALL)
			return NativeLib.SCREEN_SIZE_SMALL;
		if ((screenLayout & Configuration.SCREENLAYOUT_SIZE_NORMAL) == Configuration.SCREENLAYOUT_SIZE_NORMAL)
			return NativeLib.SCREEN_SIZE_NORMAL;
		if ((screenLayout & Configuration.SCREENLAYOUT_SIZE_LARGE) == Configuration.SCREENLAYOUT_SIZE_LARGE)
			return NativeLib.SCREEN_SIZE_LARGE;	
		if ((screenLayout & SCREENLAYOUT_SIZE_XLARGE) == SCREENLAYOUT_SIZE_XLARGE)
				return NativeLib.SCREEN_SIZE_XLARGE;
		
		throw new Exception("Unsupported screen");
	}
	
	private int GetDensity(WindowManager windowManager) throws Exception {
		DisplayMetrics metrics = new DisplayMetrics();
		windowManager.getDefaultDisplay().getMetrics(metrics);
        
        switch (metrics.densityDpi) {
		case DisplayMetrics.DENSITY_LOW:
			return NativeLib.SCREEN_DENSITY_LDPI;
		case DisplayMetrics.DENSITY_MEDIUM:
			return NativeLib.SCREEN_DENSITY_MDPI;
		case DisplayMetrics.DENSITY_HIGH:
			return NativeLib.SCREEN_DENSITY_HDPI;
		case DENSITY_XHIGH:
			return NativeLib.SCREEN_DENSITY_XHDPI;
		default:
			break;
		}
        
        throw new Exception("Unsupported screen");
	}
}
