# Ecowitt-Gateway-TCP-Parser

A lightweight and flexible Arduino library to connect to Ecowitt weather gateways over TCP and parse live sensor data. Designed for ESP32 and other microcontrollers.

## Features

-   **Simple API**: Connect to your Ecowitt gateway and retrieve data with a few lines of code.
-   **Comprehensive Parsing**: Decodes raw binary data packets into a structured format for easy use.
-   **Wide Sensor Support**: Includes definitions for a large number of Ecowitt sensors, from basic temperature and humidity to more advanced PM2.5 and lightning sensors.
-   **Cross-Platform**: Works on ESP32, Arduino, and STM32 architectures.

## Installation

1.  **Download**: Download the latest release of this library as a `.zip` file from the GitHub repository.
2.  **Arduino IDE**: In the Arduino IDE, go to `Sketch` -> `Include Library` -> `Add .ZIP Library...` and select the downloaded file.

## Usage

This example sketch demonstrates how to connect to your gateway and print the live sensor data to the Serial Monitor.

### `examples/Basic_Ecowitt_Parser/Basic_Ecowitt_Parser.ino`

```cpp
#include <WiFi.h>
#include <WiFiClient.h>
#include "Read_Ecowitt_GW1000.h"
#include <vector>

// Define your WiFi network credentials here.
// IMPORTANT: Replace the placeholder values with your actual WiFi name and password.
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Define the IP address of your Ecowitt GW1000/GW1100 here.
// IMPORTANT: Replace the placeholder with the actual IP address of your gateway.
const char* ecowittIP = "YOUR_GATEWAY_IP";

WiFiClient client;
EcowittGW1000 gw1100(client);

void setup() {
    Serial.begin(115200);

    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi!");
    Serial.print("ESP32 IP address: ");
    Serial.println(WiFi.localIP());

    delay(2000);
}

void loop() {
    if (!client.connected()) {
        Serial.println("Connection to Ecowitt GW1100 is not active. Attempting to connect...");
        if (!client.connect(ecowittIP, ecowittPort)) {
            Serial.println("Connection to Ecowitt GW1100 failed! Retrying in 5 seconds.");
            delay(5000);
            return;
        }
        Serial.println("Successfully connected to Ecowitt GW1100.");
    }
    
    Serial.println("Sending CMD_GW1000_LIVEDATA command...");
    gw1100.sendCommand(CMD_GW1000_LIVEDATA);
    
    unsigned long timeout = millis() + 5000;
    while (!client.available() && millis() < timeout) {
        delay(100);
    }
    
    if (client.available()) {
        uint8_t responseBuffer[256];
        int bytesRead = client.read(responseBuffer, sizeof(responseBuffer));
        
        if (gw1100.readAndDecode(responseBuffer, bytesRead)) {
            Serial.println("--- Decoded Data ---");
            const std::vector<EcowittSensorReading>& readings = gw1100.getReadings();
            
            for (const auto& reading : readings) {
                Serial.print(reading.description);
                Serial.print(": ");
                Serial.print(reading.value);
                Serial.print(" ");
                Serial.println(reading.units);
            }
            Serial.println("--- End of Data ---");
        } else {
            Serial.println("Failed to decode data.");
        }
    } else {
        Serial.println("Timeout: No data received from Ecowitt GW1100.");
    }

    client.stop();
    Serial.println("Connection closed. Reconnecting on next loop.");

    delay(60000);
}