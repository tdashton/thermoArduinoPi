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

    // Use WiFiClient class to create TCP connections
    if (!client.connect(host, 2021)) {
        Serial.println("connection failed");
        delay(5000);
        return; // restarts the loop
    } else {      
        client.println("CONNECT LOG");
    }

    String connectPort = connect_get_port();
    Serial.println("out" + connectPort);
    int intport = connectPort.toInt();
    Serial.println(intport);

    delay(5000);

}

String connect_get_port() {
    String portStr;
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
    Serial.println("closing connection");
    client.stop();
    return portStr;
}

String get_temp_payload() {
  return "2|SENSOR|2016-10-07 19:00:40.835130|1b-1234567|19995";
}

