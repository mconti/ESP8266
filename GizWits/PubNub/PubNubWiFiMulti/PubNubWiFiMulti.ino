/**
 * Derivato da
 * BasicHTTPClient.ino
 * presente nelgli esempio ESP8266
 * 
 * Modificato da posta@maurizioconti.com per l'uso con PubNub
 * 
 * Manuale PubNub: https://www.pubnub.com/knowledge-base/categories/rest
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

ESP8266WiFiMulti WiFiMulti;

const char* host = "pubsub.pubnub.com";
const int httpPort = 80;
char pubkey[] = "pub-c-531543bb-e12b-4db6-9c4b-f9df0fee3067";
char subkey[] = "sub-c-021829ca-e9cb-11e5-8346-0619f8945a4f";
char channel[] = "Faretto1";

const char* ssid     = "IoT";
const char* password = "Prova1234";

String pub_timeToken = "";
String sub_timeToken = "";

/**
 * Partendo dal canale che gli arriva come parametro, costruisce una roba del genere
 * http://pubsub.pubnub.com/publish/pubkey/subkey/0/canale/0/
 * e lo ritorna.
*/
String PNPubUrl(char* ch)
{
    String strUrl = String( "http://");
    strUrl += host;
    strUrl += String("/publish/");
    strUrl += pubkey + String("/");
    strUrl += subkey + String("/0/");
    strUrl += ch + String("/0/");

    return strUrl;
}

/**
 * Partendo dal canale che gli arriva come parametro, costruisce una roba del genere
 * http://pubsub.pubnub.com/subscribe/subkey/canale/0/123456789
 * dove 123456789 è il timeToken.
*/
String PNSubUrl(char* ch, String timeToken)
{
    String strUrl = String( "http://");
    strUrl += host;
    strUrl += String("/subscribe/");
    strUrl += subkey + String("/");
    strUrl += ch + String("/0/");
    strUrl += timeToken;

    return strUrl;
}

/**
 * Usando la libreria ArduinoJson, costruisce la stringa json partendo dai valori letti dal campo.
 */
String getSensorInJson(int R, int G, int B)
{
    StaticJsonBuffer<100> jsonBuffer;
    
    JsonObject& objRoot = jsonBuffer.createObject();
    objRoot["Red"] = R;
    objRoot["Blue"] = G;
    objRoot["Green"] = B;
  
    //JsonArray& data = objRoot.createNestedArray("data");
    //data.add(48.756080, 6);  // 6 is the number of decimals to print
    //data.add(2.302038, 6);   // if not specified, 2 digits are printed
    
    char buffer[200];
    objRoot.printTo(buffer, sizeof(buffer));
    
    return String( buffer );    
}

/*
 *  Verifica se una data stringa è un numero
 */
boolean isValidNumber(String str){
   for(byte i=0;i<str.length();i++)
   {
      if( isDigit(str.charAt(i)) ) 
        return true;
   }
   return false;
}

/**
 *  Estrae il timeToken dalla stringa jsonIn.
 *  Se vale 10.000 torna 0
 *  Se non corrisponde ad un numero (ad esempio c'è una stringa di errore) torna il vecchio TimeToken
 */
String decodeToken(String jsonIn, int pos, String oldToken)
{
    // 7 Giugno 2016
    // Si può migliorare, inserendo un contatore che se l'oldToken rimane uguale per un po' di volte, 
    // il sistema invia una richiesta "0" a pubnub per imporre un aggiornamento

    // Iniziamo
    // Male che vada, caschi nel valore 0 che impone a PubNub di ritornare un TimeToken aggiornato
    String retVal = "0";
    
    // Estrai il TimeToken dal json che è appena arrivato fresco fresco
    StaticJsonBuffer<1000> jsonBuffer;
    JsonArray& arrayRoot = jsonBuffer.parseArray(jsonIn);

    // Preleva il TimeToken alla posizione pos
    String token = String(arrayRoot.get<const char*>(pos));

    // Lo ritorna solo se è un numero.
    retVal = (isValidNumber( token )) ? token : oldToken; 

    // 10.000 non è un TimeToken valido ma un valore tornato da PubNub in casi speciali
    retVal = (retVal == "10000") ? "0" : retVal; 
      
    return retVal;
}

/**
 *  Estrae il payLoad dalla stringa jsonIn.
 */
String decodePayload(String jsonIn, int pos)
{
    Serial.print("json: ");
    Serial.println(jsonIn);

    String retVal = "0";
    
    DynamicJsonBuffer jsonBuffer;
    JsonArray& root = jsonBuffer.parseObject(jsonIn)[0];

    const char* inner = root.get<const char*>(0);
    Serial.print("inner: ");
    Serial.println(inner);

    JsonArray& color = jsonBuffer.parseArray(root.get<const char*>(0));
    int red = color[0];
    Serial.print("payLoad: ");
    Serial.println(red);
    
    
    
//    StaticJsonBuffer<1000> jsonBuffer;
//    JsonArray& arrayRoot = jsonBuffer.parseArray(jsonIn);
//    JsonObject& s = arrayRoot[0];
//    const char* red = s["Red"];
//    
//    Serial.print("payLoad: ");
//    Serial.println(red);
//    
//    // Preleva il PayLoad alla posizione pos
//    const char* payLoad = arrayRoot.get<const char*>(pos);
//    Serial.print("payLoad: ");
//    Serial.println(payLoad);
//    
//    JsonObject& objRoot = jsonBuffer.parseObject(payLoad);
//    if( objRoot.success() ){
//      const char* red = objRoot["Red"];
//      Serial.print( red );
//    }
      
    return retVal;
}

/**
 *    1) Usa il canale che gli arriva come parametro per costruire la prima parte dell'url PubNub
 *    2) Costruisce la stringa json da inviare a PubNub
 *    3) Spedisce tutto
 *    4) Aspetta la risposta
 *    5) Se la risposta è valida, la decodifica ed estrare il timeToken (lo memorizza come variabile globale)
 */
String PNSendData(char* ch, String msg)
{   
    Serial.print("\nPUB su : ");
    Serial.println(ch);

    String retVal="";

    // Sistema il timeToken publish in caso di valori sbagliati
    if( pub_timeToken == "10000" || pub_timeToken == "" )
      pub_timeToken = "0";
    
    // Forma l'url completo per il publish PubNub
    String strUrl = PNPubUrl( ch ) + msg;
    Serial.println(strUrl);
    
    HTTPClient http;
    http.begin( strUrl );
    http.setTimeout(200);
   
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if(httpCode > 0) {
        if(httpCode == HTTP_CODE_OK) {
            // dal server arriva roba!!!  La trasformiamo in un timeToken
            retVal = String( http.getString() );
        }
        else{
          // dal server arriva un codice ... non è un errore ma non è neanche roba buona
          retVal = String("[HTTP] GET... code: ");
          retVal += String( httpCode );
        }
    } 
    else {
        // dal server arriva un errore
        retVal = String( "[HTTP] GET... failed, error: ");
        retVal += http.errorToString(httpCode);
    }

    http.end();

    // Ricalcola il TimeToken publish
    pub_timeToken = decodeToken(retVal, 2, pub_timeToken);
    
    return retVal;
}


/**
 *  Chiama PubNub per vedere se ci sono valori sul canale  
 */
String PNGetData(char* ch, String timeToken)
{   
    String retVal = "[[], \"errore...\"]";
    
    // Sistema timeToken subscribe in caso di valori sbagliati
    if( sub_timeToken == "10000" || sub_timeToken == "" )
      sub_timeToken = "0";

    // Forma l'url completo per il subscribe PubNub
    String strUrl = PNSubUrl( ch, timeToken );
    Serial.print("\nSUB su: ");
    Serial.print(ch);
    Serial.print(", TT: ");
    Serial.println(timeToken);
    
    HTTPClient http;
    http.begin( strUrl );
    http.setTimeout(200);
   
    // start connection and send HTTP header
    int httpCode = http.GET();
    
    // httpCode will be negative on error
    if(httpCode > 0) {
        if(httpCode == HTTP_CODE_OK) {
          
            int httpSize = http.getSize();
            Serial.print("Sono arrivati:");
            Serial.print(httpSize);
            Serial.println(" byte.");
            
            if (httpSize > 0)
                retVal = http.getString();
        }
        else{
          // dal server arriva un codice ... non è un errore ma non è neanche roba buona
          retVal = String("[[],\"");
          retVal += String( httpCode );
          retVal += String("\"]");
        }
    } 
    else {
        // dal server arriva un errore
        retVal = String("[[],\"");
        retVal += http.errorToString(httpCode);
        retVal += String("\"]");
    }

    http.end();

    // Ricalcola il TimeToken subscribe
    sub_timeToken = decodeToken(retVal, 1, sub_timeToken);

    return retVal;
}

//
// al reset
//
void setup() {

    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(500);
    }

    WiFiMulti.addAP(ssid, password);
}

int oldAnalogVal = 0;

//
// main loop
//
void loop() {
   
    if((WiFiMulti.run() == WL_CONNECTED)) 
    {
      // Pubblica solo se i valori cambiano
      int analogVal = analogRead(0);
      if( oldAnalogVal != analogVal ){
        
        oldAnalogVal = analogVal;

        // Forma R, G, B in modo fittizio e li converte in json
        String strMsg = getSensorInJson(analogVal, analogVal+100, analogVal+200);
        String r1 = PNSendData("Faretto1", strMsg);
        Serial.print("ritorna: ");
        Serial.println(r1);
      }
    }
    delay(300);

    if((WiFiMulti.run() == WL_CONNECTED)) 
    {
        String r2 = PNGetData("Faretto1", sub_timeToken);
        decodePayload( r2, 0 );
//        Serial.print("ritorna: ");
//        Serial.println(r2);

//        String r1 = PNSendSensor("Faretto2");
//        Serial.print("ritorna: ");
//        Serial.println(r1);
    }
    delay(300);
}

/*
 * 

Questo è un errore dovuto al formato sbagliato della stringa json...

SUB su: Faretto2, TT: 14652860179272922
http://pubsub.pubnub.com/subscribe/sub-c-021829ca-e9cb-11e5-8346-0619f8945a4f/Faretto2/0/14652860179272922
ritorna: [[],read Timeout]
Fatal exception 28(LoadProhibitedCause):
epc1=0x4000bf0e, epc2=0x00000000, epc3=0x00000000, excvaddr=0x00000000, depc=0x00000000

Exception (28):
epc1=0x4000bf0e epc2=0x00000000 epc3=0x00000000 excvaddr=0x00000000 depc=0x00000000

ctx: cont 
sp: 3ffefd90 end: 3ffeffb0 offset: 01a0

>>>stack>>>
3ffeff30:  3ffeed60 3ffeee1b 3ffeee18 4020570a  
3ffeff40:  402010ae 3ffeee30 3ffeed48 40205816  
3ffeff50:  3fffdad0 3ffeee30 3ffeed48 40202820  
3ffeff60:  00000000 00000000 00000000 3fff065c  
3ffeff70:  0000001f 00000011 3fff0684 0000001f  
3ffeff80:  00000011 00000000 00000001 40205f15  
3ffeff90:  feefeffe 00000000 3ffeef74 40205f40  
3ffeffa0:  feefeffe feefeffe 3ffeef90 40100718  
<<<stack<<<

*/


/**
 *  Chiama PubNub per vedere se ci sono valori sul canale  
 *  
 *  Versione di prova... con timeout (ma è inutile...)
 *  
 */
//String PNGetData(char* ch, String timeToken)
//{   
//    String retVal = "[[], \"errore...\"]";
//    
//    // Sistema timeToken subscribe in caso di valori sbagliati
//    if( sub_timeToken == "10000" || sub_timeToken == "" )
//      sub_timeToken = "0";
//
//    // Forma l'url completo per il subscribe PubNub
//    String strUrl = PNSubUrl( ch, timeToken );
//    Serial.print("\nSUB su: ");
//    Serial.print(ch);
//    Serial.print(", TT: ");
//    Serial.println(timeToken);
//    Serial.println(strUrl);
//    
//    HTTPClient http;
//    http.begin( strUrl );
//   
//    // start connection and send HTTP header
//    int httpCode = http.GET();
//
//    Serial.println("ok Fatto.");
//    
//    // Number of milliseconds to wait without receiving any data before we give up
//    const int kNetworkTimeout = 500;
//
//    // Number of milliseconds to wait if no data is available before trying again
//    const int kNetworkDelay = 100;
//
//    // httpCode will be negative on error
//    if(httpCode > 0) {
//        if(httpCode == HTTP_CODE_OK) {
//          
//            int httpSize = http.getSize();
//            Serial.print("httpSize:");
//            Serial.println(httpSize);
//            
//            // Now we've got to the body, so we can print it out
//            unsigned long timeoutStart = millis();
//
//            // Whilst we haven't timed out & haven't reached the end of the body
//            while ( (httpSize > 0 && http.connected()) && ((millis() - timeoutStart) < kNetworkTimeout) )
//            {
//                if (httpSize > 0)
//                {
//                    retVal = http.getString();
//                    timeoutStart = millis();
//                }
//                else
//                {
//                    // We haven't got any data, so let's pause to allow some to
//                    // arrive
//                    delay(kNetworkDelay);
//                }
//                httpSize = http.getSize();
//            }
//        }
//        else{
//          // dal server arriva un codice ... non è un errore ma non è neanche roba buona
//          retVal = String("[[],\"");
//          retVal += String( httpCode );
//          retVal += String("\"]");
//        }
//    } 
//    else {
//        // dal server arriva un errore
//        retVal = String("[[],\"");
//        retVal += http.errorToString(httpCode);
//        retVal += String("\"]");
//    }
//
//    http.end();
//
//    // Ricalcola il TimeToken subscribe
//    sub_timeToken = getToken(retVal, 1, sub_timeToken);
//
//    return retVal;
//}


