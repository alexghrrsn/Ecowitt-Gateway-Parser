/**
 * @file ECOWITT-GATEWAY-PARSER.h
 * @brief This file contains the declaration of the Ecowitt-Gateway-Parser class and related data structures.
 *
 * It provides a class to handle TCP communication with an Ecowitt GW1000/GW1100 gateway,
 * including sending commands and decoding sensor data packets.
 */

#ifndef ECOWITT-GATEWAY-PARSER_H
#define ECOWITT-GATEWAY-PARSER_H

#include <Arduino.h>
#include <WiFiClient.h>
#include <vector>

/**
 * @brief The fixed packet header for Ecowitt communications.
 */
const uint16_t PACKET_HEADER = 0xFFFF;

/**
 * @brief The command code for requesting live sensor data.
 */
const uint8_t CMD_GW1000_LIVEDATA = 0x27;

/**
 * @brief The default TCP port used by the Ecowitt gateway for live data.
 */
const int ecowittPort = 45000;

/**
 * @struct EcowittSensorReading
 * @brief A structure to hold a single decoded sensor reading.
 */
struct EcowittSensorReading {
    uint8_t itemCode;       ///< The unique code for the sensor item.
    const char* description; ///< A human-readable description of the sensor.
    const char* units;      ///< The units of measurement.
    float value;            ///< The decoded floating-point value.
};

/**
 * @class Ecowitt-Gateway-Parser
 * @brief A class to manage the TCP connection and decode data from an Ecowitt GW1000/GW1100 gateway.
 *
 * This class handles the low-level communication protocol, including sending commands
 * and parsing the binary data stream into a structured format.
 */
class Ecowitt-Gateway-Parser {
public:
    /**
     * @brief Construct a new Ecowitt-Gateway-Parser object.
     * @param client An active WiFiClient object to use for communication.
     */
    Ecowitt-Gateway-Parser(WiFiClient& client);

    /**
     * @brief Sends a command to the Ecowitt gateway.
     * @param cmd The command code to be sent.
     */
    void sendCommand(uint8_t cmd);

    /**
     * @brief Decodes a received binary data packet from the gateway.
     * @param buffer A pointer to the buffer containing the raw bytes.
     * @param bytesRead The number of bytes in the buffer.
     * @return true if the packet was successfully decoded, false otherwise.
     */
    bool readAndDecode(uint8_t* buffer, int bytesRead);

    /**
     * @brief Gets the last set of decoded sensor readings.
     * @return A reference to a std::vector containing the decoded readings.
     */
    std::vector<EcowittSensorReading>& getReadings();

private:
    WiFiClient& _client; ///< The WiFi client instance.
    std::vector<EcowittSensorReading> _readings; ///< A vector to store the decoded readings.

    /**
     * @brief Converts a sequence of bytes into a signed 32-bit integer.
     * @param buffer A pointer to the byte array.
     * @param size The number of bytes to convert (max 4).
     * @return The resulting signed 32-bit integer.
     */
    int32_t convertBytesToInteger(uint8_t* buffer, uint8_t size);
};

#endif // READ_ECOWITT_GW1000_H