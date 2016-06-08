/**
 * Esempio di utilizzo di ESP8266 (Witty Cloud Development Board) con PubNub
 * Maurizio Conti per FabLab Romagna - posta@fablabromagna.org
 * 
 * Manuale PubNub: https://www.pubnub.com/knowledge-base/categories/rest
 * Manuale ArduinoJson: https://github.com/bblanchon/ArduinoJson/wiki/Compatibility-issues
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

String canaleComando = "command";
String canaleStato = "status";

const char* ssid     = "IoT";
const char* password = "Prova1234";

String pub_timeToken = "";
String sub_timeToken = "";

int oldAnalogVal = 0;

// LED RGB presente a bordo della scheda Witty Cloud
#define REDPIN 15
#define GREENPIN 12
#define BLUEPIN 13


/**
 * Costruisce un url per publish
 * http://pubsub.pubnub.com/publish/pubkey/subkey/0/canale/0/
 * e lo ritorna.
*/
String PNPubUrl(String ch)
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
 * Costruisce un url per subscribe
 * http://pubsub.pubnub.com/subscribe/subkey/canale/0/123456789
 * dove 123456789 è il timeToken.
*/
String PNSubUrl(String ch, String timeToken)
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
    objRoot["Green"] = G;
    objRoot["Blue"] = B;

    int len = objRoot.measureLength() + 1;
    char buffer[len];
    objRoot.printTo(buffer, len);
    
    return String( buffer );    
}

/*
 *  Verifica se la stringa è un numero
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
    DynamicJsonBuffer jsonBuffer;
    JsonArray& arrayRoot = jsonBuffer.parseArray(jsonIn);

    // Preleva il TimeToken alla posizione pos
    //String token = String(arrayRoot.get<const char*>(pos));
    String token = arrayRoot[pos].asString();

    // Lo ritorna solo se è un numero.
    retVal = (isValidNumber( token )) ? token : oldToken; 

    // 10.000 non è un TimeToken valido ma un valore tornato da PubNub in casi speciali
    retVal = (retVal == "10000") ? "0" : retVal; 
      
    return retVal;
}

/**
 *  Estrae il payLoad dalla stringa jsonIn.
 */
bool decodePayload(String jsonIn, int pos)
{
    // Arriva una roba del genere 
    // [[{"Red":236,"Blue":336,"Green":436}],"14653714002897936"]
    // A guardarlo bene si nota che è un array con dentro due elementi
    // Il primo è a sua volta un array, il secondo un numero (il timeToken)
    // A noi serve il primo elemento di root[0] quindi root[0][0]

    DynamicJsonBuffer jsonBuffer;
    JsonArray& root = jsonBuffer.parseArray(jsonIn);
    if( root.success() )
    {

      // Serial.printf( "root: %d\n", root.size() );                // Stampa 2
      // Serial.printf( "root[0]: %d\n", root[0].size() );          // Se è vuoto vale 0
      // Serial.printf( "root[0][0]: %d\n", root[0][0].size() );    // Se ci sono i tre elementi R, G, B allora vale 3
      
      if( root[0].size() > 0 )
      {    
        int R = root[0][0]["Red"];
        int G = root[0][0]["Green"];
        int B = root[0][0]["Blue"];

        analogWrite(REDPIN, R);
        analogWrite(GREENPIN, G);
        analogWrite(BLUEPIN, B);
        
        Serial.printf( "R:%d, G;%d, B:%d\n\n", R, G, B );
        return true;
      }
    }
    return false;
}

/**
 *    1) Usa il canale che gli arriva come parametro per costruire la prima parte dell'url PubNub
 *    2) Costruisce la stringa json da inviare a PubNub
 *    3) Spedisce tutto
 *    4) Aspetta la risposta
 *    5) Se la risposta è valida, la decodifica ed estrare il timeToken (lo memorizza come variabile globale)
 */
String PNSendData(String ch, String msg)
{   
    Serial.printf("%s > %s\n", ch.c_str(), msg.c_str());

    String retVal="";

    // Sistema il timeToken publish in caso di valori sbagliati
    if( pub_timeToken == "10000" || pub_timeToken == "" )
      pub_timeToken = "0";
    
    // Forma l'url completo per il publish PubNub
    String strUrl = PNPubUrl( ch ) + msg;
    
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
String PNGetData(String ch, String timeToken)
{   
    String retVal = "[[], \"errore...\"]";
    
    // Sistema timeToken subscribe in caso di valori sbagliati
    if( sub_timeToken == "10000" || sub_timeToken == "" )
      sub_timeToken = "0";

    // Forma l'url completo per il subscribe PubNub
    String strUrl = PNSubUrl( ch, timeToken );
    
    HTTPClient http;
    http.begin( strUrl );
    http.setTimeout(200);
   
    // start connection and send HTTP header
    int httpCode = http.GET();
    
    // httpCode will be negative on error
    if(httpCode > 0) {
        if(httpCode == HTTP_CODE_OK) {
          
            int httpSize = http.getSize();
            //Serial.print("Sono arrivati:");
            //Serial.print(httpSize);
            //Serial.println(" byte.");
            
            if (httpSize > 0){
                retVal = http.getString();
                Serial.printf("%s < %s %s\n",ch.c_str(), timeToken.c_str(), retVal.c_str());
            }
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

    // Campo
    pinMode(REDPIN, OUTPUT); 
    pinMode(GREENPIN, OUTPUT); 
    pinMode(BLUEPIN, OUTPUT); 

    // Seriale
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    delay(200);

    // Client WiFi 
    WiFiMulti.addAP(ssid, password);
    while((WiFiMulti.run() != WL_CONNECTED)) 
    {
        Serial.printf(".");
        Serial.flush();
        delay(200);
    }

    // La prima connessione con PubNub torna [[],"10000"] , la scartiamo
    String r2 = PNGetData(canaleStato, sub_timeToken);
    decodePayload( r2, 0 );

    // La seconda connessione viene fatta con TimeToken == 0 e serve per capire quali sia il primo TimeToken valido
    r2 = PNGetData(canaleStato, sub_timeToken);
    decodePayload( r2, 0 );

}


//
// main loop
//
void loop() {

    // Legge dal cloud
    if((WiFiMulti.run() == WL_CONNECTED)) 
    {
        // Prende da PubNub l'ultimo record, aggiorna TimeToken se necessario
        String result = PNGetData(canaleComando, sub_timeToken);

        // Interpreta ciò che è arrivato
        // Impone lo stato al campo
        // Se quello che è arrivato ha un senso (contiene un minimo di json valido 
        // ai fini di un R, G, B, allora da la conferma sul canale
        if( decodePayload( result, 0 ) ) {
          PNSendData(canaleStato, "[\"OK\"]");
        }
    }
    delay(100);

    // Scrive sul cloud
    if((WiFiMulti.run() == WL_CONNECTED)) 
    {
      // Invia sul cloud solo se i valori letti dal campo sono cambiati
      int analogVal = analogRead(0);
      if( oldAnalogVal != analogVal ){
        
        oldAnalogVal = analogVal;

        // Forma una strina json R, G, B partendo dal valore analogico letto dal campo
        String strMsg = getSensorInJson(analogVal, analogVal / 100, analogVal / 30);
        String r1 = PNSendData(canaleComando, strMsg);
      }
    }

    delay(100);
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


