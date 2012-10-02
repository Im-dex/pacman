package com.imdex.pacman;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.os.Process;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.os.Handler;
import android.os.Looper;

public class Reporter {

	private final Context mContext;
	private final Handler mHandler;
	private SurfaceView mGLView;
	private ProgressDialog mLoadingDialog;
	
	public Reporter(Context context) {
		mContext = context;
		mGLView = null;
		
		if (Looper.myLooper() != Looper.getMainLooper())
			throw new RuntimeException("Reporter must be created in UI thread only!!!");
		mHandler = new Handler();
	}
	
	public void setGLView(SurfaceView view) {
		mGLView = view;
	}
	
	public void showLoadingDialog() {
		runOnUIThread(new Runnable() {
			public void run() {
				showLoadingDialogImpl();
			}
		});
	}
	
	public void hideLoadingDialog() {
		runOnUIThread(new Runnable() {
			public void run() {
				hideLoadingDialogImpl();
			}
		});
	}
	
	public void terminateApplication(final String error) {
		if (mGLView != null) {
			mGLView.stopRendering();
		}
		
		runOnUIThread(new Runnable() {
			public void run() {
				showErrorDialog(error);
			}
		});
	}
	
	private void runOnUIThread(final Runnable runnable) {
		if (Looper.myLooper() == Looper.getMainLooper()) {
			runnable.run();
		} else {
			mHandler.post(new Runnable() {
				public void run() {
					runnable.run();
				}
			});
		}
	}
	
	private void showErrorDialog(String error) {
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
	
	private void showLoadingDialogImpl() {
		mLoadingDialog = ProgressDialog.show(mContext, "Loading", "The game is loading...Please wait.", true);
	}
	
	private void hideLoadingDialogImpl() {
		mLoadingDialog.dismiss();
		mLoadingDialog = null;
	}
}
