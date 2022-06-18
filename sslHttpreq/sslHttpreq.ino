/**
   BasicHTTPSClient.ino

    Created on: 20.08.2018

*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include <Servo.h>

const uint8_t fingerprint[20] = {0x0b, 0xae, 0xb8, 0xb3, 0xea, 0x3c, 0x84, 0x9d, 0x87, 0x80, 0x89, 0x12, 0x2c, 0x2b, 0x63, 0xb5, 0x07, 0xb1, 0x06, 0x8a};
ESP8266WiFiMulti WiFiMulti;
Servo servo;
const int Trigger = D3;
const int Echo = D2;
bool control = false;
int mil = 0;

void setup() {

  servo.attach(D1);
  servo.write(0);
  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);
  
  Serial.begin(74880);
  // Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println();
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("L0@ iPhone", "puto1234");
}

void loop() {
  // wait for WiFi connection
  if(control){
        servo.write(180);
        long t; 
        long d; 
        digitalWrite(Trigger, HIGH);
        delayMicroseconds(10); 
        digitalWrite(Trigger, LOW);
        t = pulseIn(Echo, HIGH);
        d = t/59;
        Serial.print("Distancia: ");
        Serial.print(d);      //Enviamos serialmente el valor de la distancia
        Serial.print("cm");

        if(millis() > mil)
        if(d < 10){
          control = false;
          servo.write(0);
          }
    }
  else{
      if ((WiFiMulti.run() == WL_CONNECTED)) {
    
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    
        // client->setFingerprint(fingerprint);
        // Or, if you happy to ignore the SSL certificate, then use the following line instead:
        client->setInsecure();
    
        HTTPClient https;
    
        Serial.print("[HTTPS] begin...\n");
        if (https.begin(*client, "https://proyectomixto1.azurewebsites.net/api/door")) {  // HTTPS
    
          Serial.print("[HTTPS] GET...\n");
          // start connection and send HTTP header
          int httpCode = https.GET();
    
          // httpCode will be negative on error
          if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
    
            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
              String payload = https.getString();
              Serial.println(payload);
              if(payload.equals("true")){
               control = true;
               mil = millis() + 4000;
              }
            }
          } else {
            Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
          }
                  
    
          https.end();
        } else {
          Serial.printf("[HTTPS] Unable to connect\n");
        }
      }
  }

  Serial.println("Wait 10s before next round...");
  delay(2000);
}
