#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "Secrets.h"

Adafruit_MPU6050 mpu;

int onboardLED = 2;
float gyro_X;
float gyro_X_error = 0.01;

char json_output[128];

unsigned long start_time;
unsigned long current_time;
const int period = 15000;

// decalre functions
void post_request();

void setup(){


  Serial.begin(9600);
  delay(4000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to Wifi...");
  }

  Serial.println("Connected to Wifi");

  while (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    delay(1000);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  pinMode(onboardLED, OUTPUT);
  start_time = millis();
  Serial.println("Setup complete \n");
  delay(4000);
}

void loop(){
  // digitalWrite(onboardLED, HIGH);
  // // Serial.println("This prints every few seconds");
  // delay(500);
  // digitalWrite(onboardLED, LOW);

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // // Serial.print("Rotation X: ");
  // // Serial.println(g.gyro.x);
  // // Serial.print(", Y: ");
  // // Serial.println(g.gyro.y);

  // float gyro_X_temp = g.gyro.x + 2.16;
  // if (abs(gyro_X_temp) > gyro_X_error){
  //   gyro_X += (gyro_X_temp/50.00) * (180 / 3.1415);
  // }

  // Serial.print(gyro_X_temp);
  // Serial.print("      ");
  // Serial.println(gyro_X);

  // Serial.print(", Z: ");
  // Serial.print(g.gyro.z);
  // Serial.println(" rad/s");

  // Serial.println(a.acceleration.y);
  
  post_request();

}

void post_request(){
  current_time = millis();
  if (WiFi.status() == WL_CONNECTED && current_time - start_time >= period){
    
    digitalWrite(onboardLED, HIGH);

    HTTPClient client;
    client.begin("https://reqres.in/api/users");
    client.addHeader("Content-Type", "application/json");

    // compute the required size
    const size_t CAPACITY = JSON_ARRAY_SIZE(2);

    // allocate the memory for the document
    StaticJsonDocument<CAPACITY> doc;

    // create an empty array
    JsonObject object = doc.to<JsonObject>();
    object["name"] = "Spongebob";
    object["job"] = "fry cook";
    
    serializeJson(doc, json_output);

    int httpCode = client.POST(String(json_output));  

    if (httpCode > 0){
      String payload = client.getString();

      Serial.println("StatusCode:" + String(httpCode));
      Serial.println(payload);
    }

    // reset the delay
    start_time = current_time;
    digitalWrite(onboardLED, LOW);

  }else if (current_time - start_time < period){
    Serial.println("Wating for next API call");
    delay(1000);

  }else{
    Serial.println("Connection Lost");
    delay(1000);
  }  
}