//
// posta@maurizioconti.com at Maker Inside & Out Day
// FabLab Romagna A.P.S.
//
// Mercoledì 21 Dicembre 2016 - Sede del corso di Laurea in Ingegneria e Scienze informatiche 
//
// Legge da dweet.io/dweet/for/MakerInside  i parametri red, green e blue e li impone al LED on board
// Legge la fotoresistenza locale e la spedisce a dweet
//

#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
#include <Adafruit_NeoPixel.h>

// WiFi parameters
const char* ssid = "IoT";
const char* password = "Prova1234";

// Variabili di appoggio
int R,G,B;

// i tre led
#define GizRedLed 15    // Led rosso su piattaforma Giz
#define GizGreenLed 12  // Led verde su piattaforma Giz
#define GizBlueLed 13   // Led blu su piattaforma Giz

// Sul NeoPixel
#define PIN 2

// Ring
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

void flashLed( uint32_t  c )
{
  strip.setPixelColor(0, c);
  strip.show(); 
  delay(100);
  strip.setPixelColor(0, strip.Color(0,0,0));
  strip.show(); 
  delay(100);
}

// Host dweet
const char* host = "dweet.io";

void setup() {
  
  // Start Serial
  Serial.begin(115200);
  delay(10);

  strip.begin();
  Serial.println("NeoPixel inizializzato...");
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    flashLed( strip.Color(50,0,0) );
    delay(500);
  }
  
  flashLed( strip.Color(0,0,100) );
  delay(500);
  flashLed( strip.Color(0,0,100) );
  delay(500);
  flashLed( strip.Color(0,0,100) );
  
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

    Serial.println("Rosso: " + strR);
    Serial.println("Verde: " + strG);
    Serial.println("Blu: " + strB);

    R = strR.toInt();
    G = strG.toInt();
    B = strB.toInt();

    analogWrite( GizRedLed, R);
    analogWrite( GizGreenLed, G);
    analogWrite( GizBlueLed, B);  

    for(uint16_t i=0 ; i<16 ; i++) {
      strip.setPixelColor(i, strip.Color(R, G, B));
      strip.show();
      delay(20);
    }
      
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
  //String url = String("GET /get/latest/dweet/for/MakerInside") +  " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";

  // prende gli ultimi N log 
  String url = String("GET /get/dweets/for/MakerInside") +  " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";

  // esegue la chiamata
  client.print(url);
  delay(20);
  
  // Leggi la risposta (JSON)
  String line = "";

  while(client.available()){
    line = client.readStringUntil('\r');
    DecodificaJSon( line );
    client.println(line);
  }
  
  // Rallenta. Chiamate troppo ravvicinate vengo scartate.
  delay(4000);

  //
  // Spedizione dello stato
  //
  if (!client.connect(host, httpPort)) {
    Serial.println("seconda connessione fallita");
    return;
  }
 
  // Leggi il valore locale
  //int lux = analogRead(A0);
  
  // Spedisci il valore dei lux sul cloud
  //url = String("GET /dweet/for/MakerInside?lux=") + String(lux) +  " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
  //client.print( url );
  //delay(50);
  
  // Svuota il buffer di risposta
  //while(client.available()){
  //  String line = client.readStringUntil('\r');
  //}
  
  // Rallenta. Chiamate troppo ravvicinate vengo scartate.
  //delay(4000);
}

