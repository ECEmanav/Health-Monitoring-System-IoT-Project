#include <Wire.h>
#include "ThingSpeak.h"
#include "MAX30100_PulseOximeter.h"
#include <WiFi.h> // Include the ESP32 WiFi library

// Define pins
#define SIG_PIN 7 // Modify according to your board's pinout
#define IND_PIN 10 // Modify according to your board's pinout

// Define constants
#define REPORTING_PERIOD_MS 3000

// WiFi credentials
const char* ssid = "Ujjwal Gupta";
const char* password = "qwertyui";

// ThingSpeak credentials
unsigned long myChannelNumber = 2536656;
const char* myWriteAPIKey = "UZ1PS5GSHAK6YFPL";

PulseOximeter pox;
WiFiClient client;
uint32_t tsLastReport = 0;

// Callback function when a beat is detected
void onBeatDetected() {
    // Add functionality here if needed
}

void setup() {
    Serial.begin(115200);
    delay(1000); // Allow time for Serial Monitor to start

    // Initialize the pulse oximeter
    Serial.print("Initializing pulse oximeter...");
    if (!pox.begin()) {
        Serial.println("FAILED");
        while (true); // Halt the program if initialization fails
    } else {
        Serial.println("SUCCESS");
        digitalWrite(IND_PIN, HIGH);
    }

    pox.setIRLedCurrent(MAX30100_LED_CURR_24MA);
    pox.setOnBeatDetectedCallback(onBeatDetected);

    // Connect to WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConnected to WiFi!");

    // Initialize ThingSpeak
    ThingSpeak.begin(client);
}

void loop() {
    pox.update();

    // Report sensor data every REPORTING_PERIOD_MS
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        int pulse = pox.getHeartRate();
        int spo2 = pox.getSpO2();

        Serial.print("BPM: ");
        Serial.print(pulse);
        Serial.print(" SpO2: ");
        Serial.print(spo2);
        Serial.println("%");

        // Send data to ThingSpeak
        ThingSpeak.writeField(myChannelNumber, 2, pulse, myWriteAPIKey);
        ThingSpeak.writeField(myChannelNumber, 3, spo2, myWriteAPIKey);

        tsLastReport = millis();
    }

    // Check for high signal and send SOS to ThingSpeak if triggered
    if (digitalRead(SIG_PIN) == HIGH) {
        int sos = 108; // SOS value (modify as needed)
        ThingSpeak.writeField(myChannelNumber, 1, sos, myWriteAPIKey);
    }
}
