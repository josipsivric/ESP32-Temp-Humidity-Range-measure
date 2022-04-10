#include <Arduino.h>
#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <UltrasonicSensor.h>

// SDA & SCL from I2C LCD1602 display on pins 18 & 19
#define SDA 18
#define SCL 19

// Ultrasonic sensors Trigger (output) and Echo (input) pins
#define trigger 2
#define echo 4

// LCD1602 sizes and default address
#define LCDcolumns 16
#define LCDrows 2
#define LCDaddress 0x27

// DHT11 on pin 15
#define dhtPin 15

DHTesp dht;
LiquidCrystal_I2C LCD(LCDaddress, LCDcolumns, LCDrows);
UltrasonicSensor USensor(trigger, echo);
int oldDistance = 0, newDistance = 0;
TempAndHumidity newTempAndHumidity = {0.0F, 0.0F}, oldTempAndHumidity = {0.0F, 0.0F};

void setup() {
    // Initializing LCD
    Wire.begin(SDA, SCL);
    LCD.init();
    LCD.backlight();

    // Initializing DHT11 readout
    dht.setup(dhtPin, DHTesp::DHT11);
}

void loop() {
    // Try to read temperature and humidity if sensor is ready, if not try again
    do {
        newTempAndHumidity = dht.getTempAndHumidity();
    } while (dht.getStatus() != 0);

    // Check if there is any change in temperature and humidity and only then update LCD to reduce flicker
    if (newTempAndHumidity.temperature != oldTempAndHumidity.temperature || 
        newTempAndHumidity.humidity != oldTempAndHumidity.humidity) {
        LCD.setCursor(0, 0);
        //LCD.print("                ");
        LCD.print(newTempAndHumidity.temperature);
        LCD.print(" C, ");
        LCD.print(newTempAndHumidity.humidity);
        LCD.print(" %");
        oldTempAndHumidity = newTempAndHumidity;
    }

    // Check if there is any change in distance and only then update LCD to reduce flicker
    USensor.setTemperature(newTempAndHumidity.temperature);
    newDistance = USensor.distanceInCentimeters();
    if (newDistance != oldDistance) {
        LCD.setCursor(0, 1);
        LCD.print("                ");
        LCD.setCursor(0, 1);
        LCD.print("D: ");
        LCD.print(newDistance);
        LCD.print(" cm");
        oldDistance = newDistance;
    }

    // Delay to reduce update speed, helps with readability of LCD and reduction in power usage
    delay(200);
}