// DHome Lightswitch alpha 1 for fixed hardware switches
#include <Arduino.h>
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 
#define RELAY 4 //Relay pin
#define CLED 2 //ESP8266 onboard LED PIN
#define HWSWITCH 5 //Hardware switch pin
int lightbulb = 0; // Some very useful variables
int hwswitch = 1;
int ButtonStateOld = HIGH;
int ButtonState;
int DebouncePin(void);

const char* ssid = "Your SSID here";
const char* password = "Your password here";
IPAddress IP;

ESP8266WebServer server(80);

int statLED = 0;

void handleRoot() 
{
	char temp[400];
	if (lightbulb == 0) server.send(200, "OFF", temp);
	else server.send(200, "ON", temp);
}


void lightson() {
	handleRoot();
	digitalWrite(RELAY, HIGH);
	Serial.print ("\nLights turned on");
	lightbulb = 1;
	delay(1000);
}


void lightsoff() {
	handleRoot();
	digitalWrite(RELAY, LOW);
	Serial.print ("\nLights turned off");
	lightbulb = 0;
	delay(1000);
}

void setup(void) {
	pinMode(RELAY, OUTPUT);
	pinMode(CLED, OUTPUT);
	pinMode(HWSWITCH, INPUT);
	digitalWrite(CLED, HIGH);
	ButtonStateOld = DebouncePin();
	Serial.begin(9200);
	Serial.print("Connecting to network");
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);   // Connect to WiFi network
	while (WiFi.status() != WL_CONNECTED) {    // Wait for connection
		delay(500);
		Serial.print(".");
		if (statLED == 0) 
			{statLED = 1; digitalWrite(CLED, LOW);} 
		else 
			{statLED = 0; digitalWrite(CLED, HIGH);}
	}
	server.on("/", handleRoot);
	server.on("/0", lightsoff);
	server.on("/1", lightson);

	server.begin();
	Serial.print("Connected");
	digitalWrite(CLED, LOW);
	IP = WiFi.localIP();
	Serial.println(IP);
}

int DebouncePin(void) {
	if (digitalRead(HWSWITCH) == LOW) {
		delay (25);
		if (digitalRead(HWSWITCH) == LOW) {
			Serial.println("\nBUTTON IS PRESSED!"); 
			return LOW;
		}
	}
	return HIGH;
}

void loop(void) {
	server.handleClient();
    ButtonState = DebouncePin();
	if (ButtonState != ButtonStateOld) { //Button changed its state
        ButtonStateOld = ButtonState;
		if (lightbulb == 1) lightsoff();
		else lightson();
	 }	
}
