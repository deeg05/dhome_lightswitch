//DHome lightswitch alpha 1 for nonfixed switches
#include <Arduino.h>
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 
#define RELAY 4 //Relay PIN
#define CLED 2 //ESP8266 onboard LED PIN
#define HWSWITCH 5 //Hardware switch PIN
int lightbulb = 0; //State of relay
int DebouncePin(void); //DebouncePin function announcement
int statLED = 0; //State of ESP8266 onboard LED

const char* ssid = "SSID"; //WiFi SSID
const char* password = "PWD"; //WiFi Password
IPAddress IP; //Get local IP address into IPAddress

ESP8266WebServer server(80); //Start Web Server


void handleRoot() //Display homepage
{
	char temp[400];

	snprintf(temp, 400,
	"<html>\
		<head>\
		</head>\
		<body>\
			<h1>DHome Lightswitch</h1>\
    		<a href =\"/1\"> <button>ON</button> </a>\
    		<a href =\"/0\"> <button>OFF</button> </a>\
		</body>\
	</html>"); 
	server.send(200, "text/html", temp);

}


void lightson() { //Function to turn lights on
	handleRoot(); //Display homepage
	digitalWrite(RELAY, HIGH); //Switch relay
	Serial.print ("\nLights turned on"); //Send "Lights turned on" to serial
	lightbulb = 1; //Announce that lights are on
	delay(1000); //Wait for 1 second
}


void lightsoff() {
	handleRoot(); //Display homepage
	digitalWrite(RELAY, LOW); //Switch relay
	Serial.print ("\nLights turned off"); //Send "Lights turned on" to serial 
	lightbulb = 0; //Announce that lights are off
	delay(1000); //Wait for 1 second

	
}

void setup(void) {
	pinMode(RELAY, OUTPUT); //Setup Relay PIN as output
	pinMode(CLED, OUTPUT); //Setup ESP8266 onboard LED PIN as output
	pinMode(HWSWITCH, INPUT); //Setup Hardware Switch PIN as input
	digitalWrite(CLED, HIGH); //Turn off ESP8266 onboard LED
	Serial.begin(9200); //Begin Serial at speed 9200
	Serial.print("Connecting to network"); //Send "Connecting to network" to serial
	WiFi.mode(WIFI_STA); //Set WiFi mode
	WiFi.begin(ssid, password);   // Connect to WiFi network
	while (WiFi.status() != WL_CONNECTED) {    // Wait for connection
		delay(500); //Wait 0.5 seconds
		Serial.print("."); //Send "." to serial upon ESP8266 connects to the network
		if (statLED == 0) //If ESP8266 onboard LED is off
			{statLED = 1; digitalWrite(CLED, LOW);} //Turn it ON
		else 
			{statLED = 0; digitalWrite(CLED, HIGH);} //Else turn it OFF
	}
	server.on("/", handleRoot); //URL to functions initialization. If it's IP/ then display homepage 
	server.on("/0", lightsoff); //If it's IP/0 then turn lights off and display homepage
	server.on("/1", lightson); //If it's IP/1 then turn lights on and display homepage

	server.begin(); //Start server
	Serial.print("Connected"); //Send "Connected" to serial
	digitalWrite(CLED, LOW); //Turn on ESP8266 onboard LED
	IP = WiFi.localIP();
	Serial.println(IP); //Send local IP address to serial
}

int DebouncePin(void) { //Debounce PIN function
	if (digitalRead(HWSWITCH) == LOW) {
		delay (25);
		if (digitalRead(HWSWITCH) == LOW) {  
			return LOW; //return LOW to int DebouncePin
		}
	}
	return HIGH; //return HIGH to int DebouncePin
}

void loop(void) {
	server.handleClient(); //Handle client on HTTP
	if (DebouncePin() == LOW) { //If DebouncePin is low then
		if (lightbulb == 1) lightsoff(); //If light were turned on then turn lights off 
		else lightson(); //else turn lights on
	}
	
}
