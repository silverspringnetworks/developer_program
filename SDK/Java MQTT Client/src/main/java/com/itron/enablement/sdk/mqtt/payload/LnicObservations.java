package com.itron.enablement.sdk.mqtt.payload;


/**
 * Lnic observations. This class holds the extracted observation values for a processed LNIC payload.
 *
 * @author Bob Luben
 */
public class LnicObservations
{
    // Observation timestamp
    private String      observationTimeStamp;
    private String      timestampLabel;
    // Length of data part of payload (non-header)
    private int         lengthOfDataPart;
    // Sensor Temperature
    private double      temperature = Double.NaN;
    private String      temperatureLabel;
    // Sensor Humidity
    private double      humidity = Double.NaN;
    private String      humidityLabel;
    // Sensor Shock_Sum
    private double      shocksum = Double.NaN;
    private String      shocksumLabel;
    // Tilt value (array of 3)
    private double[]    tilt;
    private String      tiltLabel = null;
    private String[]    tiltElementLabels = null;
    // Shock value (array of 3)
    private double[]    shock;
    private String      shockLabel = null;
    private String[]    shockElementLabels = null;
    // Alert value
    private int         alertValue = 0x00000000;
    private String      alertLabel = null;
    private String      alertName = null;


    // Getters and Setters
    public String getObservationTimeStamp() {
        return observationTimeStamp;
    }

    public void setObservationTimeStamp(String observationTimeStamp) {
        this.observationTimeStamp = observationTimeStamp;
    }

    public int getLengthOfDataPart() {
        return lengthOfDataPart;
    }

    public void setLengthOfDataPart(int lengthOfDataPart) {
        this.lengthOfDataPart = lengthOfDataPart;
    }

    public double getTemperature() {
        return temperature;
    }

    public void setTemperature(double temperature) {
        this.temperature = temperature;
    }

    public double getHumidity() {
        return humidity;
    }

    public void setHumidity(double humidity) {
        this.humidity = humidity;
    }

    public double getShocksum() {
        return shocksum;
    }

    public void setShocksum(double shocksum) {
        this.shocksum = shocksum;
    }

    public double[] getTilt() {
        return tilt;
    }

    public void setTilt(double[] tilt) {
        this.tilt = tilt;
    }

    public double[] getShock() {
        return shock;
    }

    public void setShock(double[] shock) {
        this.shock = shock;
    }

    public String getTimestampLabel() {
        return timestampLabel;
    }

    public void setTimestampLabel(String timestampLabel) {
        this.timestampLabel = timestampLabel;
    }

    public String getTemperatureLabel() {
        return temperatureLabel;
    }

    public void setTemperatureLabel(String temperatureLabel) {
        this.temperatureLabel = temperatureLabel;
    }

    public String getHumidityLabel() {
        return humidityLabel;
    }

    public void setHumidityLabel(String humidityLabel) {
        this.humidityLabel = humidityLabel;
    }

    public String getShocksumLabel() {
        return shocksumLabel;
    }

    public void setShocksumLabel(String shocksumLabel) {
        this.shocksumLabel = shocksumLabel;
    }

    public String getTiltLabel() {
        return tiltLabel;
    }

    public void setTiltLabel(String tiltLabel) {
        this.tiltLabel = tiltLabel;
    }

    public String[] getTiltElementLabels() {
        return tiltElementLabels;
    }

    public void setTiltElementLabels(String[] tiltElementLabels) {
        this.tiltElementLabels = tiltElementLabels;
    }

    public String getShockLabel() {
        return shockLabel;
    }

    public void setShockLabel(String shockLabel) {
        this.shockLabel = shockLabel;
    }

    public String[] getShockElementLabels() {
        return shockElementLabels;
    }

    public void setShockElementLabels(String[] shockElementLabels) {
        this.shockElementLabels = shockElementLabels;
    }

    public int getAlertValue() {
        return alertValue;
    }

    public void setAlertValue(int alertValue) {
        this.alertValue = alertValue;
    }

    public String getAlertLabel() {
        return alertLabel;
    }

    public void setAlertLabel(String alertLabel) {
        this.alertLabel = alertLabel;
    }

    public String getAlertName() {
        return alertName;
    }

    public void setAlertName(String alertName) {
        this.alertName = alertName;
    }
}
