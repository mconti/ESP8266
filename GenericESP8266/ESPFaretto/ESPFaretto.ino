/*********************************************************************************
Controller RGB via WiFi con PubNub
posta@maurizioconti.com - FabLab Romagna 2016
*********************************************************************************/
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>

// Sulla rete WiFi Locale...
const char* g_ssid      = "smd2016";
const char* g_password  = "jeCa63Mije";

// Su PubNub
const char* g_host = "pubsub.pubnub.com";
const int l_httpPort = 80;
const char* g_pubKey = "pub-c-531543bb-e12b-4db6-9c4b-f9df0fee3067";
const char* g_subKey = "sub-c-021829ca-e9cb-11e5-8346-0619f8945a4f";
const char* g_channel = "Faretto1";
String timeToken = "0";

// Sul NeoPixel
#define PIN 2

// Ring
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

// WS2811
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_RGB + NEO_KHZ800);

int R,G,B;

/*********************************************************************************
Function Name     : setup
Description       : Initialize the Serial Communication with baud 115200, Begin
                    the ESP8266 and connect to the Router and print the ip
Parameters        : void
Return            : void
*********************************************************************************/
void flashLed( uint32_t  c )
{
  strip.setPixelColor(0, c);
  strip.show(); 
  delay(100);
  strip.setPixelColor(0, strip.Color(0,0,0));
  strip.show(); 
  delay(100);
}

void setup() 
{
  
  Serial.begin(115200);
  delay(2000);
  Serial.print(".");
  Serial.print(".");
  Serial.print(".");

  Serial.println("Sistema partito!");

  strip.begin();
  Serial.println("NeoPixel inizializzato...");

  Serial.print("Connecting to SSID ");
  Serial.println(g_ssid);
  WiFi.begin(g_ssid, g_password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
    flashLed( strip.Color(50,0,0) );
  }

  flashLed( strip.Color(0,0,100) );

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void DecodificaJSon( String line )
{
  // PubNub come riscontro delle operazioni eseguite su di se, 
  // invia una risposta di 25 caratteri (compreso '/0' finale)
  
  // Una roba tipo [[],"14614039192921287"]
  // Dove il numero che vedi è l'ID dell'operazione per futuri reference. 
  // (ad esempio History dei valori o chaining delle letture)

  // La normale risposta di PubNub invece è in formato JSon con '}' terminale.
  
  // Controllando quindi l'ultimo carattere possiamo discriminare...
  if(line.endsWith("]"))
  {
    // OK PubNub ci da un ID di riscontro... procediamo con la decodifica.
    if(line.length() == 25)
    {
      char pack[26];
      line.toCharArray(pack,26);
      timeToken = pack[6];
      for( int i=7 ; i<=22 ; i++)
        timeToken += pack[i];     // timeToken è String, supporta il concatenamento tramite +=
      
      Serial.println("ID riscontro: " + timeToken);
    } 
    else
    {
      // OK. PubNub ci risponde in Json
      // Una roba del genere [[{"Red":157,"Green":0,"Blue":255}],"14614048199458969"]

      // Isoliamo la parte relativa ai colori
      int startJsonIndex = line.indexOf('{');
      int endJsonIndex = line.indexOf('}', startJsonIndex+1) + 1;
      String json = line.substring( startJsonIndex, endJsonIndex );

      Serial.print("Json dei colori: ");
      Serial.println(json);

      int startTimeToken = line.indexOf('"', endJsonIndex+1);
      int endTimeToken = line.indexOf('"', startTimeToken+1) + 1;
      timeToken = line.substring( startTimeToken, endTimeToken );

      Serial.print("ID riscontro: ");
      Serial.println(timeToken);

      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json_parsed = jsonBuffer.parseObject(json);
      if (!json_parsed.success())
      {
        Serial.println("parseObject() failed");
        return;
      }

      String strR = json_parsed["Red"];
      String strG = json_parsed["Green"];
      String strB = json_parsed["Blue"];

      R = strR.toInt();
      G = strG.toInt();
      B = strB.toInt();
      
      Serial.println("Rosso: " + strR);
      Serial.println("Verde: " + strG);
      Serial.println("Blu: " + strB);
    }
  }
}


void loop() 
{
  WiFiClient client;
  if (!client.connect(g_host, l_httpPort)) 
  {
    Serial.println("connection failed");
    return;
  }
  
  // DATA FORMAT : http://pubsub.pubnub.com/publish/pub-key/sub-key/signature/channel/callback/message
  // SUB FORMAT :  http://pubsub.pubnub.com/subscribe/sub-key/channel/callback/timetoken

  String url = "/subscribe/";
  url += g_subKey;
  url += "/";
  url += g_channel;
  url += "/0/";
  url += timeToken;
  Serial.println(url);
  Serial.println("");
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
             "Host: " + g_host + "\r\n" + 
             "Connection: close\r\n\r\n");
  delay(300);

  while(client.available()){
    String json;
    String line = client.readStringUntil('\r');

    Serial.println(line);
    DecodificaJSon( line );

    for(uint16_t i=0 ; i<16 ; i++) {
      strip.setPixelColor(i, strip.Color(R, G, B));
      strip.show();
      delay(20);
    }
  }

  Serial.println("End...");
  client.readStringUntil('\r');
}

