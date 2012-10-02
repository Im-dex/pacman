package com.imdex.pacman;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class NativeLib {

	// ATTENTION! this values was directly mapped to the native enumerations
	public static final int SCREEN_SIZE_SMALL  = 0;
	public static final int SCREEN_SIZE_NORMAL = 1;
	public static final int SCREEN_SIZE_LARGE  = 2;
	public static final int SCREEN_SIZE_XLARGE = 3;
	
	public static final int SCREEN_DENSITY_LDPI  = 0;
	public static final int SCREEN_DENSITY_MDPI  = 1;
	public static final int SCREEN_DENSITY_HDPI  = 2;
	public static final int SCREEN_DENSITY_XHDPI = 3;
	
	public static final int TOUCH_UP   = 0;
	public static final int TOUCH_DOWN = 1;
	public static final int TOUCH_MOVE = 2;
	
	static {
		System.loadLibrary("gnustl_shared");
		System.loadLibrary("nativelib");
	}
	
	private static Context mContext = null;
	private static Reporter mReporter = null;
	
	public static void setContext(Context context) {
		mContext = context;
	}
	
	public static void setReporter(Reporter reporter) {
		mReporter = reporter;
	}
	
	public static native void start();
	public static native void stop();
	public static native void pause();
	public static native void resume();
	public static native void surfaceChanged(int width, int height);
	public static native void drawFrame();
	public static native boolean touchEvent(int event, float x, float y);
	//public static native boolean keyEvent();
	
	private static Bitmap loadAssetBitmap(String fileName) {
		AssetManager manager = mContext.getAssets();
		try {
			InputStream is = manager.open(fileName);
			return BitmapFactory.decodeStream(is);
		} catch (IOException e) {
			return null;
		}
	}
	
	private static ByteBuffer loadAssetFile(String fileName)
	{
		AssetManager manager = mContext.getAssets();
		try {
			ByteArrayOutputStream os = new ByteArrayOutputStream();
			InputStream is = manager.open(fileName);
			int b = 0;
			while ((b = is.read()) != -1) {
				os.write(b);
			}
			ByteBuffer buffer = ByteBuffer.allocateDirect(os.size());
			buffer.put(ByteBuffer.wrap(os.toByteArray()));
			return buffer;
		} catch (IOException e) {
			return null;
		}
	}
	
	private static void showLoadingDialog() {
		mReporter.showLoadingDialog();
	}
	
	private static void hideLoadingDialog() {
		mReporter.hideLoadingDialog();
	}

	private static void terminateApplication() {
		mReporter.terminateApplication("Internal error");
	}
}
