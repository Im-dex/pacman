package com.imdex.pacman;

import android.app.AlertDialog;
import android.os.Process;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.os.Handler;
import android.os.Looper;

public class ErrorReporter {

	private final Context mContext;
	private final Handler mHandler;
	private SurfaceView mGLView;
	
	public ErrorReporter(Context context) {
		mContext = context;
		mGLView = null;
		
		if (Looper.myLooper() != Looper.getMainLooper())
			throw new RuntimeException("Error reporter must be created in UI thread only!!!");
		mHandler = new Handler();
	}
	
	public void setGLView(SurfaceView view) {
		mGLView = view;
	}
	
	public void terminateApplication(final String error) {
		if (mGLView != null) {
			mGLView.stopRendering();
		}
		
		if (Looper.myLooper() == Looper.getMainLooper()) {
			showDialog(error);
		} else {
			mHandler.post(new Runnable() {
				public void run() {
					showDialog(error);
				}
			});
		}
	}
	
	private void showDialog(String error) {
		AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
		builder.setMessage(error)
			   .setIcon(android.R.drawable.ic_delete)
			   .setTitle("Fatal error occurred")
			   .setPositiveButton("Ok", new OnClickListener() {
				   public void onClick(DialogInterface dialog, int which) {
					   Process.killProcess(Process.myPid());
				   }
			   	});
		
		AlertDialog dialog = builder.create();
		dialog.show();
	}
}
