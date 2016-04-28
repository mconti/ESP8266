/*********************************************************************************
Pubnub Subscribe Sample
*********************************************************************************/
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

const char* g_ssid      = "IOT";
const char* g_password  = "Prova1234";
const char* g_host      = "pubsub.pubnub.com";
const char* g_pubKey    = "pub-c-65914541-3bbd-4fa9-979d-ffe4b018be8f";
const char* g_subKey    = "sub-c-12fa7c6c-8534-11e5-83e3-02ee2ddab7fe";
const char* g_channel   = "P1";
String      timeToken    = "0";

/*********************************************************************************
Function Name     : setup
Description       : Initialize the Serial Communication with baud 115200, Begin
                    the ESP8266 and connect to the Router and print the ip
Parameters        : void
Return            : void
*********************************************************************************/

void setup() 
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(g_ssid);
  
  WiFi.begin(g_ssid, g_password);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

/*********************************************************************************
Function Name     : loop
Description       : Connect to the PUBNUB REST API with TCP/IP Connection 
                    and Subscribe the sample data from the pubnub
Parameters        : void
Return            : void
*********************************************************************************/

void loop() 
{
  WiFiClient client;
  const int l_httpPort = 80;
  if (!client.connect(g_host, l_httpPort)) 
  {
    Serial.println("connection failed");
    return;
  }
  //DATA FORMAT : http://pubsub.pubnub.com /publish/pub-key/sub-key/signature/channel/callback/message
  //SUB FORMAT :  http://pubsub.pubnub.com /subscribe/sub-key/channel/callback/timetoken

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
  delay(100);

  while(client.available())
  {
    String json;
    String line = client.readStringUntil('\r');
    Serial.println(line);
    
    if(line.endsWith("]"))
    {
      if(line.length() == 25)
      {
        Serial.print("!");
        char pack[26];
        line.toCharArray(pack,26);
        timeToken = pack[6];
        for(int i = 7;i<=22;i++)
        {
          timeToken += pack[i];
        }
        
        Serial.println(timeToken);
      } 
      else
      {
        Serial.println("?");
        char bufferr[150];
        String data = line;
        
        Serial.println(line);
        
        int startJsonIndex = line.indexOf('{');
        int endJsonIndex = line.indexOf('}', startJsonIndex+1) + 1;
        json = line.substring( startJsonIndex, endJsonIndex );

        int startTimeToken = line.indexOf('"', endJsonIndex+1);
        int endTimeToken = line.indexOf('"', startTimeToken+1) + 1;
        timeToken = line.substring( startTimeToken, endTimeToken );

        Serial.println(timeToken);
        Serial.println(json);

        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& json_parsed = jsonBuffer.parseObject(json);
        if (!json_parsed.success())
        {
          Serial.println("parseObject() failed");
          return;
        }

        String val1 = json_parsed["Val1"];
        String val2 = json_parsed["Val2"];
        
        Serial.println(val1);
        Serial.println(val2);
      }
    }
  }

  Serial.println("closing connection");
  delay(2000);
  client.readStringUntil('\r');
}

//End of the Code
