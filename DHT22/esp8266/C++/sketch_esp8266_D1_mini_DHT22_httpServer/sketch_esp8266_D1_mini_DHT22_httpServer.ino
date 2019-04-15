#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

// Select the type of sensor, in this case DHT22
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// DHT22:   PIN 1:  VCC     connect with 3,3v or 5V , both voltages are supported due to internal resistor.
//          PIN 2:  Data    use the pin D2 or D6, you can use D8 too but this results in that you can not flash the ESP8266 anymore. 
//          PIN 3:  NC      not connected
//          PIN 4:  GND     connect ground


// Wifi credentials:
const char* ssid = "add ssid here";
const char* password = "add password here";

// http server on port 80
ESP8266WebServer server(80);

// DHT Sensor
// Define the PIN used for the "Data" wire of the sensor
// In our case we use D2  

uint8_t DHTPin = D2; 
               

// Initialize DHT sensor.

DHT dht(DHTPin, DHTTYPE);                

float Temperature;
float Humidity;
 
void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(DHTPin, INPUT);
  
  Serial.println((String)"Connecting to Sensor type DHT" + DHTTYPE + " on Pin" + DHTPin);
  
  dht.begin();              

  Serial.println((String)"Connecting to WiFi  ssid: " + ssid);

//  Define static IPAddress of the ESP8266 board

  IPAddress ip(192, 168, 178, 72);
  IPAddress gateway(192, 168, 178, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(192, 168, 178, 1);
  WiFi.config(ip, dns, gateway, subnet);

//connect to your local wi-fi network

  WiFi.begin(ssid, password);

//check wi-fi status
  
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

}
void loop() {
  
  server.handleClient();
  
}

void handle_OnConnect() {

// get the sensor data every 60 seconds  
// get the temperature and humidity values

  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity 
  server.send(200, "text/html", SendHTML(Temperature,Humidity)); 

// print temperature and humidity on console
// output language: german

  Serial.println((String)"Temperatur: " + Temperature);
  Serial.println((String)"Luftfeuchtigkeit: " + Humidity);
  delay(60000);
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}


// Format and send sensor information to the http server

String SendHTML(float Temperaturestat,float Humiditystat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Weather Report</title>\n";
  ptr +="<meta http-equiv=\"refresh\" content=\"2\" >";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 NodeMCU DHT22 Office Sensor</h1>\n";
  
  ptr +="<p>Temperatur: ";
  ptr +=(int)Temperaturestat;
  ptr +=" C</p>";
  ptr +="<p>Luftfeuchtigkeit: ";
  ptr +=(int)Humiditystat;
  ptr +=" %</p>";
  
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
