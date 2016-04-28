//
// posta@maurizioconti.com at School Maker Day
// FabLab Romagna A.P.S.
//
// Sabato 30 Aprile 2016 - School Maker Day - Opificio Golinelli
//
// Legge da dweet.io/dweet/for/SchoolMakerDay  i parametri red, green e blue e li impone al LED on board
// Legge la fotoresistenza locale e la spedisce a dweet
//

#include <ArduinoJson.h>
#include "ESP8266WiFi.h"

// WiFi parameters
const char* ssid = "IoT";
const char* password = "Prova1234";

// Variabili di appoggio
int R,G,B;

// i tre led
#define GizRedLed 15    // Led rosso su piattaforma Giz
#define GizGreenLed 12  // Led verde su piattaforma Giz
#define GizBlueLed 13   // Led blu su piattaforma Giz

// Host dweet
const char* host = "dweet.io";

void setup() {
  
  // Start Serial
  Serial.begin(115200);
  delay(10);
  
  // We start by connecting to a WiFi network
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

void DecodificaJSon( String line )
{
 
  if( line.indexOf('{') == 1 ) {
    Serial.println(line);
    
    char json[1000];
    line.toCharArray(json, 1000, 0);
    
    StaticJsonBuffer<1000> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    if (!root.success())
    {
      Serial.println("parseObject() failed");
      return;
    }

    // vedi https://dweet.io/ per la struttura della risposta
    String strR = root["with"][0]["content"]["red"].asString();
    String strG = root["with"][0]["content"]["green"].asString();
    String strB = root["with"][0]["content"]["blue"].asString();

    analogWrite( GizRedLed, strR.toInt());
    analogWrite( GizGreenLed, strG.toInt());
    analogWrite( GizBlueLed, strB.toInt());  
  }
  else
    Serial.print("!");

}

//
//  main loop
//
void loop() {

  // Crea la connessione
  WiFiClient client;
  const int httpPort = 80;

  //
  // Lettura dello stato dal cloud
  //
  if (!client.connect(host, httpPort)) {
    Serial.println("prima connessione fallita");
    return;
  }

  // prende l'ultimo log
  //String url = String("GET /get/latest/dweet/for/SchoolMakerDayRGB") +  " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";

  // prende gli ultimi N log 
  String url = String("GET /get/dweets/for/SchoolMakerDayRGB") +  " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";

  // esegue la chiamata
  client.print(url);
  delay(20);
  
  // Leggi la risposta (JSON)
  String line = "";
  while(client.available()){
    line = client.readStringUntil('\r');
    DecodificaJSon( line );
  }
  
  // Rallenta. Chiamate troppo ravvicinate vengo scartate.
  delay(5000);

  //
  // Spedizione dello stato
  //
  if (!client.connect(host, httpPort)) {
    Serial.println("seconda connessione fallita");
    return;
  }
 
  // Leggi il valore locale
  int lux = analogRead(A0);
  
  // Spedisci il valore dei lux sul cloud
  url = String("GET /dweet/for/SchoolMakerDayLDR?lux=") + String(lux) +  " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
  client.print( url );
  delay(50);
  
  // Svuota il buffer di risposta
  while(client.available()){
    String line = client.readStringUntil('\r');
  }
  
  // Rallenta. Chiamate troppo ravvicinate vengo scartate.
  delay(5000);
}

