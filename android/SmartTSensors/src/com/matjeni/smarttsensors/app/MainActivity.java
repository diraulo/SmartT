package com.matjeni.smarttsensors.app;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.content.Context;
import android.graphics.Paint;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.TextView;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.GoogleMap.InfoWindowAdapter;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;

public class MainActivity extends FragmentActivity {

	private static final String TAG = "MainActivity";
	private static final double LAT = -25.85;
	private static final double LON = 27.5;

	// private static String type="";
	// private static String id="";
	// private static String power="";
	// private static Double temp=0.00;
	GoogleMap mGmap;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_map);
		mGmap = ((SupportMapFragment) (getSupportFragmentManager()
				.findFragmentById(R.id.map))).getMap();
		mGmap.setMapType(GoogleMap.MAP_TYPE_NORMAL);
		mGmap.getUiSettings().setCompassEnabled(true);
		mGmap.getUiSettings().setZoomControlsEnabled(true);
		mGmap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(LAT,
				LON), 10));

		// Setting a custom info window adapter for the google map
		//mGmap.setInfoWindowAdapter(new CustomInfoWindowAdapter());
		new MapDataLoaderTask(this).execute();

		
		
	}

	public class CustomInfoWindowAdapter implements InfoWindowAdapter
	{
		Map<Marker, InfoDTO> mData;
		     
        public CustomInfoWindowAdapter( Map<Marker, InfoDTO> data)
        {
           mData = data;
        }
        @Override
		public View getInfoWindow(Marker arg0) 
		{
		  return null;
		}

		// Defines the contents of the InfoWindow
		@Override
		public View getInfoContents(Marker marker) 
		{

			// Getting view from the layout file info_window_layout
			View v = getLayoutInflater().inflate(R.layout.info_window, null);
			
			

			// Getting the position from the marker
			LatLng latLng = marker.getPosition();

			TextView tvtit = (TextView) v.findViewById(R.id.title);

			//tvtit.setPaintFlags(tvtit.getPaintFlags() 
			//		Paint.UNDERLINE_TEXT_FLAG);
			
			tvtit.setText(marker.getTitle());
			
			// Getting reference to the TextView to set latitude
			
			InfoDTO dto =  mData.get(marker);
			
			((TextView) v.findViewById(R.id.sensor_id)).setText("Sendor-ID: "+dto.getId());
			((TextView) v.findViewById(R.id.sensor_type)).setText("Sensor-Type: "+dto.getType());
			
			if(dto.getType().equalsIgnoreCase("power")){
				 ((TextView) v.findViewById(R.id.power)).setText("Power is "+dto.getPower()); 
			}
			else{
				((TextView) v.findViewById(R.id.temp)).setText("Temperature is "+dto.getTemp()); 	
			}
		    		   
		//	((TextView) v.findViewById(R.id.humidity)).setText("Humidity : "+dto.getHumidity());
		//	((TextView) v.findViewById(R.id.light)).setText("Light :"+dto.getLight());
		//	((TextView) v.findViewById(R.id.cox)).setText("COX :"+dto.getCox());

			// Returning the view containing InfoWindow contents
			return v;
		}
	}
	public class MapDataLoaderTask extends AsyncTask<Void, Void, JSONArray> {
		private Context mContext;

		public MapDataLoaderTask(Context context) {
			mContext = context;
		}

		@Override
		protected JSONArray doInBackground(Void... arg0) {

			try {
				URLConnection con = new URL("http://www.corrie206.co.za/couchdb/timestamp?descending=true").openConnection();
				InputStream is = con.getInputStream();
				int i;
				ByteArrayOutputStream baos = new ByteArrayOutputStream();
				while ((i = is.read()) != -1) {
					baos.write(i);
				}
				String s = new String(baos.toByteArray());
				Log.i("[main]", s);
				JSONObject readings = new JSONObject(s);
				return readings.getJSONArray("rows");

			} catch (MalformedURLException e) 
			{
				Log.e(TAG, "Unsupported URL type", e);
			}
			catch (IOException e) 
			{
				Log.e(TAG, "IO Error", e);
			} 
			catch (JSONException e) 
			{
				Log.e(TAG, "JSON Error", e);
			}
			return null;		// Setting the latitude
			// tvLat.setText("ID: " + id);

			// Setting the longitude
			// tvLng.setText("Type: "+ type);
		}

		
		@Override
		protected void onPostExecute(JSONArray results) 
		{
			Map<Marker, InfoDTO> mData =  new HashMap<Marker, InfoDTO>();
			
			List<String> sensorIDs = new ArrayList<String>();
			
			super.onPostExecute(results);
			try {
				if (results != null) {							 
					
					Log.i(TAG,results.toString());				

					for (int j = 0; j < results.length(); j++) {
						
						Log.i(TAG, "");
						
						String id = results.getJSONObject(j)
								.getJSONObject("value")
								.getJSONObject("readings").getString("id");
						
						if(sensorIDs.contains(id)){
							Log.i(TAG," Sensor ID "+id+" Already Processed, Skipping");
							//continue;
						}
						else{
							sensorIDs.add(id);
						}
						
						String type = results.getJSONObject(j)
								.getJSONObject("value")
								.getJSONObject("readings").getString("type");
						double lat = results.getJSONObject(j)
								.getJSONObject("value")
								.getJSONObject("readings").getDouble("lat");
						double lon = results.getJSONObject(j)
								.getJSONObject("value")
								.getJSONObject("readings").getDouble("lon");
						
						double temp = 0;
						String powerStatus = "UNKNOWN";
						
						JSONObject tempPowerJO = results.getJSONObject(j).getJSONObject("value").getJSONObject("readings");
						
						if(tempPowerJO.has("temperature")){
							temp = tempPowerJO.getDouble("temperature");
							Log.i(TAG,"Temperature reading received : "+temp);
						}
						else if(tempPowerJO.has("power")){
							powerStatus = tempPowerJO.getString("power") ;
							Log.i(TAG,"Power status received : "+powerStatus);
						}
						
						JSONObject jsonObjHumidity = results.getJSONObject(j).getJSONObject("value").getJSONObject("readings");
						String humidityStr = "NO-VALUE";
						if(jsonObjHumidity.has("humidity")){
							humidityStr = String.valueOf(jsonObjHumidity.getDouble("humidity"));
							Log.i(TAG, "Got Humidity = "+humidityStr);
						}
						
//						double humidity = (results.getJSONObject(j)	.getJSONObject("value").getJSONObject("readings").has("humidity")?
//													results.getJSONObject(j).getJSONObject("value")
//													.getJSONObject("readings").getDouble("humidity"): 0);
						
						double light = (results.getJSONObject(j).getJSONObject("value").getJSONObject("readings").has("light")?
								results.getJSONObject(j).getJSONObject("value").getJSONObject("readings").getDouble("light"):0);
						
						double cox = (results.getJSONObject(j).getJSONObject("value").getJSONObject("readings").has("cox")?
								results.getJSONObject(j).getJSONObject("value").getJSONObject("readings").getDouble("cox"):0);
						

						LatLng latlng = new LatLng(lat, lon);

						Marker marker = mGmap.addMarker(new MarkerOptions()
								.position(latlng)
								.title("Smart City")
								.snippet("ID:" + id)
								.icon(BitmapDescriptorFactory
										.defaultMarker(BitmapDescriptorFactory.HUE_GREEN)));
											
						InfoDTO dto  =  new InfoDTO(id, type, powerStatus.toUpperCase(), String.valueOf(temp),humidityStr,String.valueOf(light),String.valueOf(cox)); 
						mData.put(marker,dto);
																		
						Log.i(TAG,  "lat" + lat + "lon" + lon);	 
					}
					
					mGmap.setInfoWindowAdapter(new CustomInfoWindowAdapter(mData));

				}

			} catch (Exception e)
			{
				Log.e(TAG,"Error in parsing" , e);
			}

		}
	}
}
