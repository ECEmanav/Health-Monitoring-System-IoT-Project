#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ThingSpeak.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
char auth[] = "YOUR_BLYNK_AUTH_TOKEN";
const char* thingSpeakApiKey = "YOUR_THINGSPEAK_API_KEY";
const char* server = "api.thingspeak.com";

PulseOximeter pox;
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000;

void onBeatDetected() {
    Serial.print("Heart rate: ");
    Serial.print(pox.getHeartRate());
    Serial.print(" bpm / SpO2: ");
    Serial.print(pox.getSpO2());
    Serial.println(" %");
}

void setup() {
    Serial.begin(115200);
    if (!pox.begin()) {
        Serial.println("Failed to initialize MAX30100 sensor");
        while (1);
    }
    pox.setOnBeatDetectedCallback(onBeatDetected);
    Blynk.begin(auth, ssid, password);
    ThingSpeak.begin(WiFi);
}

void loop() {
    pox.update();
    if (millis() - lastSendTime > sendInterval) {
        lastSendTime = millis();
        float heartRate = pox.getHeartRate();
        float spo2 = pox.getSpO2();
        if (heartRate > 0) {
            ThingSpeak.setField(1, heartRate);
            ThingSpeak.setField(2, spo2);
            ThingSpeak.writeFields(thingSpeakApiKey);
            Blynk.virtualWrite(V0, heartRate);
            Blynk.virtualWrite(V1, spo2);
        }
    }
    Blynk.run();
    delay(100);
}
