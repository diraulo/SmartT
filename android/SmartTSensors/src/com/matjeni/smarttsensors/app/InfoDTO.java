package com.matjeni.smarttsensors.app;

public class InfoDTO {

	private String id;
	private String type;
	private String power; 
	private String temp;
	private String humidity;
	private String light;
	private String cox;
		
	public InfoDTO(String id, String type, String power, String temp, String humidity,
			String light, String cox) {
		super();
		this.id = id;
		this.type = type;
		this.temp = temp;
		this.humidity = humidity;
		this.light = light;
		this.cox = cox;
		this.power = power;
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public String getTemp() {
		return temp;
	}
	public void setTemp(String temp) {
		this.temp = temp;
	}
	public String getHumidity() {
		return humidity;
	}
	public void setHumidity(String humidity) {
		this.humidity = humidity;
	}
	public String getLight() {
		return light;
	}
	public void setLight(String light) {
		this.light = light;
	}
	public String getCox() {
		return cox;
	}
	public void setCox(String cox) {
		this.cox = cox;
	}
	
    public String getPower(){ return this.power;}

}
