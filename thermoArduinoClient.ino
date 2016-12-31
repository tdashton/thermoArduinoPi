/*
 * ThermoArduinoClient
 */

// Adapted from OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

#include <ESP8266WiFi.h>
#include <OneWire.h>
#include "config.h"

OneWire  ds(14);  // on pin 10 (a 4.7K resistor is necessary)

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

    String connectPort = connect_get_port();
    if(connectPort.length() == 0) {
        Serial.println("get port failed");
        delay(5000);
        return; // restarts the loop
    }
    Serial.println("should re-connect to:" + connectPort);
    int intport = connectPort.toInt();
    Serial.println(intport);
    // Use WiFiClient class to create TCP connections
    if (!client.connect(host, intport)) {
        Serial.println("connection failed");
        delay(5000);
        return; // restarts the loop
    }
    for(;;) {
        client.println(get_temp_payload("kitchen", get_temp()));
        delay(5000);
    }
}

String connect_get_port() {
    // Use WiFiClient class to create TCP connections
    if (!client.connect(host, 2021)) {
        Serial.println("connection failed");
        return "";
    } else {
        client.println("CONNECT LOG");
    }

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

int get_temp(void) {
    byte i;
    byte present = 0;
    byte type_s;
    byte data[12];
    byte addr[8];
    float celsius, fahrenheit;

    if ( !ds.search(addr)) {
        Serial.println("No more addresses.");
        Serial.println();
        ds.reset_search();
        delay(250);
        return -1;
    }

    Serial.print("ROM =");
    for( i = 0; i < 8; i++) {
        Serial.write(' ');
        Serial.print(addr[i], HEX);
    }

    if (OneWire::crc8(addr, 7) != addr[7]) {
        Serial.println("CRC is not valid!");
        return -1;
    }
    Serial.println();

    // the first ROM byte indicates which chip
    switch (addr[0]) {
        case 0x10:
            Serial.println("  Chip = DS18S20");  // or old DS1820
            type_s = 1;
            break;
        case 0x28:
            Serial.println("  Chip = DS18B20");
            type_s = 0;
            break;
        case 0x22:
            Serial.println("  Chip = DS1822");
            type_s = 0;
            break;
        default:
            Serial.println("Device is not a DS18x20 family device.");
            return -1;
    }

    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1);        // start conversion, with parasite power on at the end

    delay(1000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.

    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE);         // Read Scratchpad

    Serial.print("  Data = ");
    Serial.print(present, HEX);
    Serial.print(" ");
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
        data[i] = ds.read();
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s) {
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10) {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } else {
        byte cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
        else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
        else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
        //// default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;
    fahrenheit = celsius * 1.8 + 32.0;
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");
    return (int)(celsius * 1000);
}

String get_temp_payload(String name, int temperature) {
    return "2|SENSOR|2016-10-07 19:00:40.835130|" + name + "|" + temperature;
}
