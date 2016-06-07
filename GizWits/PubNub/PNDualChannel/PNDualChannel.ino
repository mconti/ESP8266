/**
 * Derivato da
 * PubNumWiFiMulti.ino
 * 
 * Modificato da posta@maurizioconti.com per l'uso con due canali PubNub
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

ESP8266WiFiMulti WiFiMulti;

const char* host = "pubsub.pubnub.com";
const int httpPort = 80;
char pubkey[] = "demo";
char subkey[] = "demo";
char channel[] = "hello_world";

const char* ssid     = "IoT";
const char* password = "Prova1234";

void loop() {

    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        Serial.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        //http.begin("http://192.168.1.12/test.html"); //HTTP
        //http.begin("http://requestb.in/16y50iv1"); //HTTP
        //http.begin("http://pubsub.pubnub.com/publish/pub-c-531543bb-e12b-4db6-9c4b-f9df0fee3067/sub-c-021829ca-e9cb-11e5-8346-0619f8945a4f/0/Faretto1/0/%22Hello%20World%22"); 

        String strUrl = String( "http://");
        strUrl += host;
        strUrl += String("/publish/pub-c-531543bb-e12b-4db6-9c4b-f9df0fee3067/sub-c-021829ca-e9cb-11e5-8346-0619f8945a4f/0/Faretto1/0/" );

        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["sensor"] = "gps";
        root["time"] = 1351824120;

        JsonArray& data = root.createNestedArray("data");
        data.add(48.756080, 6);  // 6 is the number of decimals to print
        data.add(2.302038, 6);   // if not specified, 2 digits are printed
        
        char buffer[200];
        root.printTo(buffer, sizeof(buffer));
        
        Serial.println("publishing message: ");
        Serial.println( strUrl + String(buffer) );
        
        http.begin( strUrl + String(buffer) );

        Serial.println("Spedito!");
        delay(300);

        Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(10000);
}

