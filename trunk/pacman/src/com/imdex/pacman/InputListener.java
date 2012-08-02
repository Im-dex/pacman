package com.imdex.pacman;

import android.content.DialogInterface;
import android.content.DialogInterface.OnKeyListener;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

public class InputListener implements OnTouchListener, OnKeyListener {
 	
	public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
		//return NativeLib.keyEvent();
		return false;
	}

	public boolean onTouch(View v, MotionEvent event) {
		return NativeLib.touchEvent(ConvertAction(event), event.getX(), event.getY());
	}
	
	private int ConvertAction(MotionEvent event) {
		switch (event.getAction()) {
		case MotionEvent.ACTION_UP:
			return NativeLib.TOUCH_UP;
		case MotionEvent.ACTION_DOWN:
			return NativeLib.TOUCH_DOWN;
		case MotionEvent.ACTION_MOVE:
			return NativeLib.TOUCH_MOVE;
		default:
			return -1;
		}

	}
	
}
