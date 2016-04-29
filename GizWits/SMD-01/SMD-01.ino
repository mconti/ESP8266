//
// posta@maurizioconti.com at School Maker Day
// Opificio Golinelli - Sabato 30 Aprile 2016
// FabLab Romagna A.P.S.
//
#include "ESP8266WiFi.h"

// WiFi parameters
const char* ssid = "IoT";
const char* password = "Prova1234";

// Host
const char* host = "dweet.io";

void setup() {
  
  // Start Serial
  Serial.begin(115200);
  delay(10);
  
  // Si parte!
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
 
  // Usiamo WiFiClient per la connessione al web
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // Leggiamo il valore analogico della LDR
  int lux = analogRead(A0);
  
  // Costruisco l'url per dweet.io
  String url = String("GET /dweet/for/SchoolMakerDayLDR?lux=") + 
               String(lux) + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n";
  
  // il Log qui...
  // https://dweet.io/follow/SchoolMakerDayLDR 
  client.print(url);
  delay(10);
  
  // Leggo tutto quello che mi arriva come risposta...
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
    
  // Ogni 5 secondi rifaciamo il giro.
  delay(5000);
}
