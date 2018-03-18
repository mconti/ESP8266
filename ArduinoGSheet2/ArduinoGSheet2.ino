#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include "DebugMacros.h"

// for stack analytics
extern "C" {
#include <cont.h>
  extern cont_t g_cont;
}

// Fill ssid and password with your network credentials
const char* ssid = "NuovoBuscone";
const char* password = "Prova1234";

// URL chiamato durante il funzionamento
// https://script.google.com/macros/s/AKfycbwVHad4668Vne5Rvfb4BIH6wMWqHS5WsJAqutmW_jG9lJEA6Wkt/exec

const char* host = "script.google.com";
const char* GScriptId = "AKfycbwVHad4668Vne5Rvfb4BIH6wMWqHS5WsJAqutmW_jG9lJEA6Wkt";
const int httpsPort = 443;

// Lanciare questo comando sul mac per ottenere il fingerprint
// echo | openssl s_client -connect script.google.com:443 | openssl x509 -fingerprint -noout
const char* fingerprint = "54:7B:BB:F6:8D:57:0D:1F:FD:6F:30:37:63:17:24:59:6A:DF:97:FF";

HTTPSRedirect* client = nullptr;
String urlForPOST = String("/macros/s/") + GScriptId + "/exec?cal";

String payload_base =  "{\"command\": \"appendRow\", \"sheet_name\": \"Foglio1\", \"values\": ";
String payload = "";

int Connect() {

  Serial.println("");
  Serial.print("Connecting with host ");
  Serial.println(host);
  
  client = new HTTPSRedirect(httpsPort);
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");

  if ( client->connect(host, httpsPort) ) {
    Serial.println("Connected");
    
    if ( client->verify(fingerprint, host) ) {
      Serial.println("Certificate match.");
      Serial.println("");
      return 0;
    }
    else {
      Serial.println("Certificate mis-match");
      Serial.println("");
      return 1;
    }
  } 
  else {
    Serial.println("!not connected...");
    return 2;
  }  
}

void setup() {

  Serial.begin(115200);
  Serial.flush();

  Serial.println("");
  Serial.println("");
  Serial.println("Si parte!");
    
  Serial.println();
  Serial.print("Connecting to WiFi ssid: ");
  Serial.println(ssid);
  // flush() is needed to print the above (connecting...) message reliably, 
  // in case the wireless connection doesn't go through
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("WiFi connected (");
  Serial.print(WiFi.localIP());
  Serial.println(")");
}

void loop() {
  
  static int cnt = 0;
  
  Serial.printf("Free heap: %u\n", ESP.getFreeHeap());
  //Serial.printf("unmodified stack   = %4d\n", cont_get_free_stack(&g_cont));
  
  if ( client == nullptr ){
    Connect();    
  }
  
  if ( client->connected() ){

    // Spedisce valore con GET (visualizza valore)
    String url = String( "/macros/s/") + GScriptId + "/exec?value=" + cnt;

    String msg = String(host) + url;
    Serial.print("Chiamo(");
    Serial.print(cnt);
    Serial.print(") ");
    Serial.println(msg);

    client->GET(url, host);

    // Spedisce valore con POST (fa il grafico..)
    payload = payload_base + "\"" + cnt + "," + ESP.getFreeHeap() + "\"}";
    client->POST( urlForPOST, host, payload, false );

    cnt++;
  }
  else{
    Serial.println("Non siamo connessi...");
    client = nullptr;
  }

  if ( cnt >= 10)  {
    ESP.deepSleep(500); // 20e6 is 20 seconds
    //delete client;
    //client = nullptr;
    //cnt = 0;
    //return;  
  }
  
  delay(100);
                          
}

