// DHome Lightswitch 0.1 for fixed hardware switches
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "main.h"
#include <SimpleTimer.h>

#define RELAY 4 //Relay pin
#define CLED 2 //ESP8266 onboard LED PIN
#define HWSWITCH 5 //Hardware switch pin

#define CNT_CONECT_WAIT 100 // Connection wait time

const char* ssid = "SSID";
const char* password = "PWD";

IPAddress IP;
SimpleTimer timer;

ESP8266WebServer server(80);

int statLED = 0;


void ledBlink() {
  if (statLED == 0) 
    {statLED = 1; digitalWrite(CLED, LOW);} 
  else
    {statLED = 0; digitalWrite(CLED, HIGH);}
}

void connectWifi() {
  int count;
  
  if (WiFi.status() != WL_CONNECTED) {
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(ssid);

      WiFi.mode(WIFI_STA); 
      WiFi.begin(ssid, password);

      count=0;
      while (WiFi.status() != WL_CONNECTED) {
        delay(50);
        Serial.print(".");
        Serial.print(WiFi.status());
        count++;
        if (count >= CNT_CONECT_WAIT){Serial.println("WiFi not connected");break;}
      }
    }

    if ( WiFi.status() == WL_CONNECTED ) {
      IP = WiFi.localIP(); //Initialise IP address
      Serial.print("IPv4 address: ");
      Serial.println(IP); // Print IP address
    }
}

void handleRoot() {
	//char temp[400];
	if (lightbulb == 1) server.send(200, "text/plain", "ON");
	else server.send(200, "text/plain", "OFF");
}


void lightsOn() {
	digitalWrite(RELAY, HIGH);
	Serial.print ("\nLights turned on");
	lightbulb = 1;
  handleRoot();
	delay(1000);
}


void lightsOff() {
	digitalWrite(RELAY, LOW);
	Serial.print ("\nLights turned off");
	lightbulb = 0;
  handleRoot();
	delay(1000);
}

void setup(void) {
	pinMode(RELAY, OUTPUT); //Initialise digital pins
	pinMode(CLED, OUTPUT);
	pinMode(HWSWITCH, INPUT);
 
	digitalWrite(CLED, HIGH); //Turn on onboard LED
 
	ButtonStateOld = debouncePin(); // Initialise old button state
  	lightbulb = 0; // Default light status
  
	Serial.begin(9600);
	Serial.print("Connecting to network");
  
  	connectWifi();
  	timer.setInterval(30000L, connectWifi);

	server.on("/", handleRoot); // On what GET request to what
	server.on("/0", lightsOff);
	server.on("/1", lightsOn);

	server.begin();
 
	if (WiFi.status() == WL_CONNECTED) Serial.print("\nConnected\n");
  else Serial.print("\nNot Connected, will retry\n");
  
	digitalWrite(CLED, LOW); // Turn off onboard LED
  
	IP = WiFi.localIP(); //Initialise IP address
	Serial.println(IP);
}

int debouncePin(void) {
	if (digitalRead(HWSWITCH) == LOW) {
		delay (25);
		if (digitalRead(HWSWITCH) == LOW) {
			return LOW;
		}
	}
	return HIGH;
}

void loop(void) {
	server.handleClient();
  ButtonState = debouncePin();
  
	if (ButtonState != ButtonStateOld) { // Check if button changed its state
        ButtonStateOld = ButtonState;
		if (lightbulb == 1) lightsOff(); else lightsOn(); // Turn relay on or off
	 }

  timer.run();
}
