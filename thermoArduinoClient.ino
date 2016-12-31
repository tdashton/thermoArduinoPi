/*
 * ThermoArduinoClient
 */

#include <ESP8266WiFi.h>
#include "config.h"

const unsigned int MAX_INPUT = 20;

WiFiClient client;

void setup() {
    Serial.begin(115200);
    delay(100);
     
    // We start by connecting to a WiFi network
     
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
     
/**
 * Main program loop
 */
void loop() {
    Serial.print("connecting to ");
    Serial.println(host);
    String portStr;

    // Use WiFiClient class to create TCP connections
    if (!client.connect(host, 2021)) {
        Serial.println("connection failed");
        delay(5000);

        String test = "NEGOTIATE:2021";
        int index = test.indexOf(":");
        Serial.println("portLine.substring(index + 1)");
        Serial.println(test.substring(index + 1));
        portStr = test.substring(index + 1);
        Serial.println("portStr");
        Serial.println(portStr);
        
        return; // restarts the loop
    } else {      
        client.println("CONNECT LOG");
    }

    bool portRead = false;
    // if bytes are available we read them
    
    while(portRead == false) {
      do {
          String ackLine = client.readStringUntil('\n');
          Serial.println(ackLine);
          String portLine = client.readStringUntil('\n');
          Serial.println(portLine);
          int index = portLine.indexOf(":");
    //      Serial.println(portLine.substring(index + 1));
          portStr = portLine.substring(index + 1);
          Serial.println("loop-port-while" + portStr);
          portRead = true;
      } while (client.available());
      Serial.println("port-loop-for" + portStr);
      delay(1000);
    }
    
    Serial.println("port-loop" + portStr);

    Serial.println("disconnecting.");
    client.stop();

    Serial.println("out" + portStr);
    int intport = portStr.toInt();
    Serial.println(intport);

    Serial.println("closing connection");
    client.stop();

    delay(5000);

//    // We now create a URI for the request
//    String url = "/testwifi/index.html";
//    Serial.print("Requesting URL: ");
//    Serial.println(url);
//    // This will send the request to the server
//    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
//    "Host: " + host + "\r\n" +
//    "Connection: close\r\n\r\n");
//    delay(500);
//    // Read all the lines of the reply from server and print them to Serial
//    while(client.available()){
//        String line = client.readStringUntil('\r');
//        Serial.print(line);
//    }
//    Serial.println();
//    Serial.println("closing connection");
}

String get_temp_payload() {
  return "2|SENSOR|2016-10-07 19:00:40.835130|1b-1234567|19995";
}

