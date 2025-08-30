/**
 * @file Read_Ecowitt_GW1000.cpp
 * @brief This file contains the implementation of the EcowittGW1000 class.
 *
 * @section license License
 * This library is released under the MIT License. For details, see the accompanying LICENSE file.
 */

#include "Read_Ecowitt_GW1000.h"

EcowittGW1000::EcowittGW1000(WiFiClient& client) : _client(client) {
    // Constructor initializes the client reference
}

/**
 * @struct SensorItem
 * @brief A local structure to define a sensor type and its data size.
 */
struct SensorItem {
    uint8_t itemCode;
    const char* description;
    const char* units;
    uint8_t size;
};

/**
 * @brief An array of all known sensor item definitions as of V1.6.9 of the GW1000 documentation.
 */
static const SensorItem sensorItems[] = {
    {0x01, "Indoor Temperature", "°C", 2},
    {0x02, "Outdoor Temperature", "°C", 2},
    {0x03, "Dew point", "°C", 2},
    {0x04, "Wind chill", "°C", 2},
    {0x05, "Heat index", "°C", 2},
    {0x06, "Indoor Humidity", "%", 1},
    {0x07, "Outdoor Humidity", "%", 1},
    {0x08, "Absolutely Barometric", "hPa", 2},
    {0x09, "Relative Barometric", "hPa", 2},
    {0x0A, "Wind Direction", "°", 2},
    {0x0B, "Wind Speed", "m/s", 2},
    {0x0C, "Gust Speed", "m/s", 2},
    {0x0D, "Rain Event", "mm", 2},
    {0x0E, "Rain Rate", "mm/h", 2},
    {0x0F, "Rain Gain", "mm", 2},
    {0x10, "Rain Day", "mm", 2},
    {0x11, "Rain Week", "mm", 2},
    {0x12, "Rain Month", "mm", 4},
    {0x13, "Rain Year", "mm", 4},
    {0x14, "Rain Totals", "mm", 4},
    {0x15, "Light", "lux", 4},
    {0x16, "UV", "uW/m²", 2},
    {0x17, "UVI", "index", 1},
    {0x18, "Date and time", "N/A", 6},
    {0x19, "Day max wind", "m/s", 2},
    {0x1A, "Temperature 1", "°C", 2},
    {0x1B, "Temperature 2", "°C", 2},
    {0x1C, "Temperature 3", "°C", 2},
    {0x1D, "Temperature 4", "°C", 2},
    {0x1E, "Temperature 5", "°C", 2},
    {0x1F, "Temperature 6", "°C", 2},
    {0x20, "Temperature 7", "°C", 2},
    {0x21, "Temperature 8", "°C", 2},
    {0x22, "Humidity 1", "%", 1},
    {0x23, "Humidity 2", "%", 1},
    {0x24, "Humidity 3", "%", 1},
    {0x25, "Humidity 4", "%", 1},
    {0x26, "Humidity 5", "%", 1},
    {0x27, "Humidity 6", "%", 1},
    {0x28, "Humidity 7", "%", 1},
    {0x29, "Humidity 8", "%", 1},
    {0x2A, "PM2.5 Air Quality Sensor", "µg/m³", 2},
    {0x2B, "Soil Temperature 1", "°C", 2},
    {0x2C, "Soil Moisture 1", "%", 1},
    {0x2D, "Soil Temperature 2", "°C", 2},
    {0x2E, "Soil Moisture 2", "%", 1},
    {0x2F, "Soil Temperature 3", "°C", 2},
    {0x30, "Soil Moisture 3", "%", 1},
    {0x31, "Soil Temperature 4", "°C", 2},
    {0x32, "Soil Moisture 4", "%", 1},
    {0x33, "Soil Temperature 5", "°C", 2},
    {0x34, "Soil Moisture 5", "%", 1},
    {0x35, "Soil Temperature 6", "°C", 2},
    {0x36, "Soil Moisture 6", "%", 1},
    {0x37, "Soil Temperature 7", "°C", 2},
    {0x38, "Soil Moisture 7", "%", 1},
    {0x39, "Soil Temperature 8", "°C", 2},
    {0x3A, "Soil Moisture 8", "%", 1},
    {0x3B, "Soil Temperature 9", "°C", 2},
    {0x3C, "Soil Moisture 9", "%", 1},
    {0x3D, "Soil Temperature 10", "°C", 2},
    {0x3E, "Soil Moisture 10", "%", 1},
    {0x3F, "Soil Temperature 11", "°C", 2},
    {0x40, "Soil Moisture 11", "%", 1},
    {0x41, "Soil Temperature 12", "°C", 2},
    {0x42, "Soil Moisture 12", "%", 1},
    {0x43, "Soil Temperature 13", "°C", 2},
    {0x44, "Soil Moisture 13", "%", 1},
    {0x45, "Soil Temperature 14", "°C", 2},
    {0x46, "Soil Moisture 14", "%", 1},
    {0x47, "Soil Temperature 15", "°C", 2},
    {0x48, "Soil Moisture 15", "%", 1},
    {0x49, "Soil Temperature 16", "°C", 2},
    {0x4A, "Soil Moisture 16", "%", 1},
    {0x4C, "All sensor lowbatt", "N/A", 16},
    {0x4D, "PM2.5 24H Average 1", "N/A", 2},
    {0x4E, "PM2.5 24H Average 2", "N/A", 2},
    {0x4F, "PM2.5 24H Average 3", "N/A", 2},
    {0x50, "PM2.5 24H Average 4", "N/A", 2},
    {0x51, "PM2.5 Air Quality Sensor 2", "µg/m³", 2},
    {0x52, "PM2.5 Air Quality Sensor 3", "µg/m³", 2},
    {0x53, "PM2.5 Air Quality Sensor 4", "µg/m³", 2},
    {0x58, "Leak Sensor 1", "N/A", 1},
    {0x59, "Leak Sensor 2", "N/A", 1},
    {0x5A, "Leak Sensor 3", "N/A", 1},
    {0x5B, "Leak Sensor 4", "N/A", 1},
    {0x60, "Lightning distance", "KM", 1},
    {0x61, "Lightning happened time", "UTC", 4},
    {0x62, "Lightning counter", "N/A", 4},
    {0x63, "Temperature 1", "°C", 3},
    {0x64, "Temperature 2", "°C", 3},
    {0x65, "Temperature 3", "°C", 3},
    {0x66, "Temperature 4", "°C", 3},
    {0x67, "Temperature 5", "°C", 3},
    {0x68, "Temperature 6", "°C", 3},
    {0x69, "Temperature 7", "°C", 3},
    {0x6A, "Temperature 8", "°C", 3},
    {0x6C, "Heap Free", "N/A", 4},
    {0x70, "CO2 Sensor", "N/A", 16},
    {0x71, "PM2.5 AQI", "N/A", 1},
    {0x72, "Leaf Wetness 1", "N/A", 1},
    {0x73, "Leaf Wetness 2", "N/A", 1},
    {0x74, "Leaf Wetness 3", "N/A", 1},
    {0x75, "Leaf Wetness 4", "N/A", 1},
    {0x76, "Leaf Wetness 5", "N/A", 1},
    {0x77, "Leaf Wetness 6", "N/A", 1},
    {0x78, "Leaf Wetness 7", "N/A", 1},
    {0x79, "Leaf Wetness 8", "N/A", 1},
    {0x7A, "Rain Priority", "N/A", 1},
    {0x7B, "Radiation compensation", "N/A", 1},
    {0x80, "Piezo Rain Rate", "N/A", 2},
    {0x81, "Piezo Event Rain", "N/A", 2},
    {0x82, "Piezo Hourly Rain", "N/A", 2},
    {0x83, "Piezo Daily Rain", "N/A", 4},
    {0x84, "Piezo Weekly Rain", "N/A", 4},
    {0x85, "Piezo Monthly Rain", "N/A", 4},
    {0x86, "Piezo yearly Rain", "N/A", 4},
    {0x87, "Piezo Gain", "N/A", 2},
    {0x88, "RST Rain Time", "N/A", 3},
};

const size_t numSensorItems = sizeof(sensorItems) / sizeof(sensorItems[0]);

/**
 * @brief Sends a command to the Ecowitt gateway.
 * @param cmd The command code to be sent.
 */
void EcowittGW1000::sendCommand(uint8_t cmd) {
    uint8_t packet[5];
    packet[0] = (PACKET_HEADER >> 8) & 0xFF;
    packet[1] = PACKET_HEADER & 0xFF;
    packet[2] = cmd;
    packet[3] = 0x03;
    uint8_t checksum = cmd + packet[3];
    packet[4] = checksum;

    _client.write(packet, sizeof(packet));
}

/**
 * @brief Converts a sequence of bytes into a signed 32-bit integer.
 * @param buffer A pointer to the byte array.
 * @param size The number of bytes to convert (max 4).
 * @return The resulting signed 32-bit integer.
 */
int32_t EcowittGW1000::convertBytesToInteger(uint8_t* buffer, uint8_t size) {
    int32_t value = 0;
    for (int i = 0; i < size; ++i) {
        value = (value << 8) | buffer[i];
    }
    return value;
}

/**
 * @brief Decodes a received binary data packet from the gateway.
 * @param responseBuffer A pointer to the buffer containing the raw bytes.
 * @param bytesRead The number of bytes in the buffer.
 * @return true if the packet was successfully decoded, false otherwise.
 */
bool EcowittGW1000::readAndDecode(uint8_t* responseBuffer, int bytesRead) {
    _readings.clear();

    if (bytesRead < 5) {
        return false;
    }

    if (responseBuffer[0] != 0xFF || responseBuffer[1] != 0xFF || responseBuffer[2] != 0x27) {
      return false;
    }

    int index = 5;
    
    while (index < bytesRead) {
        uint8_t itemCode = responseBuffer[index++];
        
        // Special handling for ITEM_SENSOR_CO2
        if (itemCode == 0x70) {
            // 1. tf_co2 (short C x10)
            int16_t tf_co2 = (int16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x70, "CO2 Temperature", "°C", tf_co2 / 10.0});
            index += 2;
            
            // 2. humi_co2 (unsigned char %)
            uint8_t humi_co2 = responseBuffer[index++];
            _readings.push_back({0x70, "CO2 Humidity", "%", (float)humi_co2});
            
            // 3. pm10_co2 (unsigned short ug/m3 x10)
            uint16_t pm10_co2 = (uint16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x70, "CO2 PM10", "ug/m³", pm10_co2 / 10.0});
            index += 2;
            
            // 4. pm10_24h_co2 (unsigned short ug/m3 x10)
            uint16_t pm10_24h_co2 = (uint16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x70, "CO2 PM10 24h Avg", "ug/m³", pm10_24h_co2 / 10.0});
            index += 2;

            // 5. pm25_co2 (unsigned short ug/m3 x10)
            uint16_t pm25_co2 = (uint16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x70, "CO2 PM2.5", "ug/m³", pm25_co2 / 10.0});
            index += 2;
            
            // 6. pm25_24h_co2 (unsigned short ug/m3 x10)
            uint16_t pm25_24h_co2 = (uint16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x70, "CO2 PM2.5 24h Avg", "ug/m³", pm25_24h_co2 / 10.0});
            index += 2;

            // 7. co2 (unsigned short ppm)
            uint16_t co2 = (uint16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x70, "CO2", "ppm", (float)co2});
            index += 2;

            // 8. co2_24h (unsigned short ppm)
            uint16_t co2_24h = (uint16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x70, "CO2 24h Avg", "ppm", (float)co2_24h});
            index += 2;

            // 9. co2_batt (u8)
            uint8_t co2_batt = responseBuffer[index++];
            _readings.push_back({0x70, "CO2 Battery", "N/A", (float)co2_batt});
            continue;
        }

        // Special handling for ITEM_PM25_AQI
        if (itemCode == 0x71) {
            // These are all 2-byte integer values
            // 1. aqi_pm25 (int)
            int16_t aqi_pm25 = (int16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x71, "AQI (PM2.5)", "index", (float)aqi_pm25});
            index += 2;
            
            // 2. aqi_pm25_24h (int)
            int16_t aqi_pm25_24h = (int16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x71, "AQI (PM2.5, 24h Avg)", "index", (float)aqi_pm25_24h});
            index += 2;
            
            // 3. aqi_pm25_in (int)
            int16_t aqi_pm25_in = (int16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x71, "AQI (PM2.5, Indoor)", "index", (float)aqi_pm25_in});
            index += 2;

            // 4. aqi_pm25_in_24h (int)
            int16_t aqi_pm25_in_24h = (int16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x71, "AQI (PM2.5, Indoor, 24h Avg)", "index", (float)aqi_pm25_in_24h});
            index += 2;

            // 5. aqi_pm25_aqin (int)
            int16_t aqi_pm25_aqin = (int16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x71, "AQI (PM2.5, AQIN Sensor)", "index", (float)aqi_pm25_aqin});
            index += 2;
            
            // 6. aqi_pm25_24h_aqin (int)
            int16_t aqi_pm25_24h_aqin = (int16_t)convertBytesToInteger(&responseBuffer[index], 2);
            _readings.push_back({0x71, "AQI (PM2.5, AQIN Sensor, 24h Avg)", "index", (float)aqi_pm25_24h_aqin});
            index += 2;
            continue;
        }

        // Handle other item codes
        bool itemFound = false;
        for (size_t i = 0; i < numSensorItems; i++) {
            if (sensorItems[i].itemCode == itemCode) {
                itemFound = true;
                if (index + sensorItems[i].size <= bytesRead) {
                    float value;
                    if (sensorItems[i].itemCode >= 0x01 && sensorItems[i].itemCode <= 0x05) {
                        // Temperature values need to be divided by 10
                        value = (int16_t)convertBytesToInteger(&responseBuffer[index], sensorItems[i].size) / 10.0;
                    } else if (sensorItems[i].itemCode == 0x06 || sensorItems[i].itemCode == 0x07) {
                        // Humidity values are simple integers
                        value = (float)responseBuffer[index];
                    } else if (sensorItems[i].itemCode == 0x0B || sensorItems[i].itemCode == 0x0C) {
                        // Wind speed values need to be divided by 10
                        value = (uint16_t)convertBytesToInteger(&responseBuffer[index], sensorItems[i].size) / 10.0;
                    } else if (sensorItems[i].itemCode >= 0x0D && sensorItems[i].itemCode <= 0x14) {
                        // Rain values need to be divided by 10
                        value = (uint32_t)convertBytesToInteger(&responseBuffer[index], sensorItems[i].size) / 10.0;
                    } else if (sensorItems[i].itemCode == 0x16) {
                        // UV needs to be divided by 10
                        value = (uint16_t)convertBytesToInteger(&responseBuffer[index], sensorItems[i].size) / 10.0;
                    } else {
                        // For other values, assume a simple integer for now.
                        value = (float)convertBytesToInteger(&responseBuffer[index], sensorItems[i].size);
                    }
                    _readings.push_back({itemCode, sensorItems[i].description, sensorItems[i].units, value});
                    index += sensorItems[i].size;
                }
                break;
            }
        }
        
        if (!itemFound) {
            // An unknown item code was found, stop decoding.
            return false;
        }
    }
    return true;
}

/**
 * @brief Gets the last set of decoded sensor readings.
 * @return A reference to a std::vector containing the decoded readings.
 */
std::vector<EcowittSensorReading>& EcowittGW1000::getReadings() {
    return _readings;
}