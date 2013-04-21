package com.matjeni.smarttsensors.app;

import com.google.android.gms.maps.GoogleMap;

import android.app.Activity;
import android.os.Bundle;

public class MapActivity extends Activity {
	
	private static final String TAG = "MapActivity";
	private GoogleMap gmap;
	@Override
	protected void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_map);
	}
	
	

}
