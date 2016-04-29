//
// posta@maurizioconti.com at School Maker Day
// FabLab Romagna A.P.S.
//
// Sabato 30 Aprile 2016 - School Maker Day - Opificio Golinelli
//
// Sulla pressione del pulsante GPIO0, attiva CH_PD e trasmette si Gweet.io

#include <ArduinoJson.h>
#include "ESP8266WiFi.h"

// WiFi parameters
const char* ssid = "IoT";
const char* password = "Prova1234";

// Variabili di appoggio
int G;
int Running = 0;

// il led
#define GizGreenLed 12  // Led verde su piattaforma Giz

// Host dweet
const char* host = "dweet.io";

void setup() {

  pinMode(0, OUTPUT);
  digitalWrite( 0, HIGH);

  // GPIO2 uscita verso Led
  pinMode(2, OUTPUT);
  digitalWrite( 2, HIGH );
  delay(100);
  digitalWrite( 2, LOW );
  delay(100);


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
    String strG = root["with"][0]["content"]["pulsante"].asString();
    if( strG != "" )
      G = strG.toInt();
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
    String url = String("GET /get/latest/dweet/for/SchoolMakerDayNerf") +  " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
  
    // prende gli ultimi N log 
    //String url = String("GET /get/dweets/for/SchoolMakerDayNerf") +  " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
  
    // esegue la chiamata
    client.print(url);
    delay(20);
    
    // Leggi la risposta (JSON)
    String line = "";
    while(client.available()){
      line = client.readStringUntil('\r');
      DecodificaJSon( line );
    }
    
    //
    // Spedizione dello stato
    //
    if (!client.connect(host, httpPort)) {
      Serial.println("seconda connessione fallita");
      return;
    }
  
    if( G == 0 )
      G=1;
    else
      G=0;
    
    // Spedisci il valore sul cloud
    url = String("GET /dweet/for/SchoolMakerDayNerf?pulsante=") + String(G) +  " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
    client.print( url );
    delay(50);
    
    // Svuota il buffer di risposta
    while(client.available()){
      String line = client.readStringUntil('\r');
    }    

    // Spegni CH_PD
    digitalWrite( 0, LOW );
}

