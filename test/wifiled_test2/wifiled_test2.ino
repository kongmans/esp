#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer esp8266_server(80);




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(D4, OUTPUT);
  wifiMulti.addAP("布凡", "20202020");
  wifiMulti.addAP("bufan", "20202020");


  Serial.println("LINKING......");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(1000);
    Serial.print(i++);
    Serial.print("");
  }

  Serial.println('\n');
  Serial.print("CONNECTED TO:\t");
  Serial.println(WiFi.SSID());
  Serial.print("IP ADDRESS:\t");
  Serial.println(WiFi.localIP());

  esp8266_server.begin();
  esp8266_server.on("/", HTTP_GET, handleRoot);
  esp8266_server.on("/led1", HTTP_POST, handleLED1);
  esp8266_server.on("/led1", HTTP_POST, handleLED2);
  esp8266_server.onNotFound(handleNotFound);

  Serial.println("HTTP ESP8266_SERVER IS STARTED!");
}

void loop(void) {
  // put your main code here, to run repeatedly:
  esp8266_server.handleClient();
}

void handleRoot() {
  esp8266_server.send(200, "text/html", "<form action=\"/led1\" method=\"post\"> <input style=\"background-color:green;height:100px;width:200px;display: block;\" type=\"submit\" value=\"LED LIGHT1\"/></form><br>");
  esp8266_server.send(200, "text/html", "<form action=\"/led2\" method=\"post\"> <input style=\"background-color:red;height:200px;width:200px;display: block;\" type=\"submit\" value=\"LED LIGHT2\"/></form>");
}
void handleLED1() {
  digitalWrite(D4, !digitalRead(D4));
  esp8266_server.sendHeader("location", "/");
  esp8266_server.send(303);
}
void handleLED2() {
  digitalWrite(D3, !digitalRead(D3));
  esp8266_server.sendHeader("location", "/");
  esp8266_server.send(303);
}

void handleNotFound() {
    esp8266_server.send(404,"text/plain", "404:NOT FOUND");
  
}
