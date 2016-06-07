#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <ArduinoJson.h>

const char* host = "pubsub.pubnub.com";
const int httpPort = 80;
char pubkey[] = "demo";
char subkey[] = "demo";
char channel[] = "hello_world";

const char* ssid     = "IoT";
const char* password = "Prova1234";

void setup() {

  Serial.begin(115200);
  Serial.println("Serial set up");
  Serial.println();
  Serial.println();
  
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

void loop() {
  
  WiFiClient client;
  if (!client.connect(host, httpPort)) 
  {
    Serial.println("connection failed");
    return;
  }

  
// GET http://pubsub.pubnub.com/publish/demo/demo/0/hello_world/0/%22Hello%20World%22 HTTP/1.1
// Host: pubsub.pubnub.com
// Connection: keep-alive
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.79 Safari/537.36
// Accept: text/html,application/json

  //http://requestb.in/16y50iv1
  host = "requestb.in";

//  String strMsg = String( "GET http://");
//  strMsg += host;
//  strMsg += String("/publish/demo/demo/0/hello_world/0/[Hello: 100] HTTP/1.1\r\n" );
//  strMsg += String("Host: ");
//  strMsg += host + String("\n\r");
//  strMsg += String("User-Agent: ESP8266\r\n");
//  strMsg += String("Accept: text/html,application/json\r\n");
//  strMsg += String("Connection: close\r\n\r\n");
  
  String strMsg = String( "GET http://");
  strMsg += host;
  strMsg += String("/16y50iv1/inspect HTTP/1.1\r\n" );
  strMsg += String("Host: ");
  strMsg += host + String("\n\r");
  strMsg += String("User-Agent: ESP8266\r\n");
  strMsg += String("Accept: text/html,application/json\r\n");
  strMsg += String("Connection: close\r\n\r\n");

  //String strMsg = String("http://pubsub.pubnub.com/publish/" + pubkey + "/" + subkey + "/0/" + channel + "/0/" + "Hello World");
  
  Serial.println("publishing message: ");
  Serial.println(strMsg);
  client.print(strMsg);
  Serial.println("Spedito!");
  delay(300);

  String line = String("");
  while(client.available()){
    line += client.readStringUntil('\r');
  }

  Serial.println( line );
  client.stop();
  delay(2000);

}
