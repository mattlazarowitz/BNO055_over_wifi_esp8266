#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <ArduinoJson.h>


/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (50)

Adafruit_BNO055 bno = Adafruit_BNO055(55);
WiFiServer server(80);
const char* ssid = "";
const char* password = "";

/**************************************************************************/
/*
Displays some basic information on this sensor from the unified
sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
	sensor_t sensor;
	bno.getSensor(&sensor);
	Serial.println("------------------------------------");
	Serial.print("Sensor:       "); Serial.println(sensor.name);
	Serial.print("Driver Ver:   "); Serial.println(sensor.version);
	Serial.print("Unique ID:    "); Serial.println(sensor.sensor_id);
	Serial.print("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
	Serial.print("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
	Serial.print("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
	Serial.println("------------------------------------");
	Serial.println("");
	delay(500);
}

char* textResponse(char* buffer, sensors_event_t& event)
{
	String x(event.orientation.x, 3);
	String y(event.orientation.y, 3);
	String z(event.orientation.z, 3);
	String ax(event.acceleration.x, 3);
	String ay(event.acceleration.y, 3);
	String az(event.acceleration.z, 3);

	snprintf(buffer, 499, "x=%s y=%s z=%s ax=%s, ay=%s, az=%s\n", 
      x.c_str(), y.c_str(), z.c_str(), ax.c_str(), ay.c_str(), az.c_str());
	return buffer;
}

bool readRequest(WiFiClient& client) {
	bool currentLineIsBlank = true;
	while (client.connected()) {
		if (client.available()) {
			char c = client.read();
			if (c == '\n' && currentLineIsBlank) {
				return true;
			}
			else if (c == '\n') {
				currentLineIsBlank = true;
			}
			else if (c != '\r') {
				currentLineIsBlank = false;
			}
		}
	}
	return false;
}


/**************************************************************************/
/*
Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{
	Serial.begin(115200);
	Serial.println("Orientation Sensor Test w/ wifi"); Serial.println("");
	Wire.begin(4,5); //one of the libraries messes up the pins so we need to explicitly set them.
	/* Initialise the sensor */
	if (!bno.begin())
	{
		/* There was a problem detecting the BNO055 ... check your connections */
		Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
		while (1);
	}

	delay(200);

	/* Display some basic information on this sensor */
	displaySensorDetails();
	WiFi.mode(WIFI_STA);
	delay(1000);
	// Connect to WiFi network
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(100);
		Serial.print(".");
	}
	Serial.println(" ");
	Serial.print("IP address ");
	Serial.print(WiFi.localIP());
	Serial.println(" ");
	server.begin();


}

/**************************************************************************/
/*
Arduino loop function, called once 'setup' is complete (your own code
should go here)
*/
/**************************************************************************/
void loop(void)
{
	/* Get a new sensor event */
	sensors_event_t event;
	WiFiClient client = server.available();
	bno.getEvent(&event);

	char text[500];

	if (client) {
		bool success = readRequest(client);
		if (success) {
			uint8_t status = client.status();
			while (status == ESTABLISHED) {
				Serial.print("~");
				bno.getEvent(&event);
				textResponse(text, event);
				client.print(text);
				delay(BNO055_SAMPLERATE_DELAY_MS);
				status = client.status();
			}
		}
		delay(1);
		client.stop();
	}
	delay(BNO055_SAMPLERATE_DELAY_MS);
}
