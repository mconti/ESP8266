/*
  PubNub sample client
  Versione Maurizio Conti - posta@maurizioconti.com
  
  This sample client will publish raw (JSON pre-encoded) PubNub messages.

  Circuit:
  * Ethernet shield attached to pins 10, 11, 12, 13
  * (Optional.) Analog sensors on pins A0 to A5.
  * (Optional.) LED on pin 9 for success indication.

  created 23 October 2012
  by Petr Baudis

  https://github.com/pubnub/pubnub-api/tree/master/arduino
  This code is in the public domain.
  */
  
extern "C" {
  #include <ctype.h>
  #include <stddef.h>
  #include <stdarg.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <strings.h>
}

int 
_DEFUN (strncasecmp, (s1, s2, n),
  _CONST char *s1 _AND
  _CONST char *s2 _AND
  size_t n)
{
  _CONST unsigned char *ucs1 = (_CONST unsigned char *) s1;
  _CONST unsigned char *ucs2 = (_CONST unsigned char *) s2;
  int d = 0;
  for ( ; n != 0; n--)
    {
      _CONST int c1 = tolower(*ucs1++);
      _CONST int c2 = tolower(*ucs2++);
      if (((d = c1 - c2) != 0) || (c2 == '\0'))
        break;
    }
Serial.print(d);
  return d;
}

size_t
_DEFUN (strspn, (s1, s2),
  _CONST char *s1 _AND
  _CONST char *s2)
{
  _CONST char *s = s1;
  _CONST char *c;

  while (*s1)
    {
      for (c = s2; *c; c++)
  {
    if (*s1 == *c)
      break;
  }
      if (*c == '\0')
  break;
      s1++;
    }
Serial.print("!");
  return s1 - s;
}

#include <ArduinoJson.h>
#include <SPI.h>
#include <PubNubMauri.h>

const int pubLedPin = 13;

char pubkey[] = "demo";
char subkey[] = "demo";
char channel[] = "hello_world";

const char* ssid     = "IoT";
const char* password = "Prova1234";

void setup()
{
	pinMode(pubLedPin, OUTPUT);
	digitalWrite(pubLedPin, LOW);

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

	PubNub.begin(pubkey, subkey);
	Serial.println("PubNub set up");
}

void flash(int ledPin)
{
	/* Flash LED three times. */
	for (int i = 0; i < 3; i++) {
		digitalWrite(ledPin, HIGH);
		delay(100);
		digitalWrite(ledPin, LOW);
		delay(100);
	}
}

void loop()
{
	WiFiClient *client;

	//char msg[64] = "{\"analog\":";
  //sprintf(msg + strlen(msg), "%d", analogRead(0));
  //strcat(msg, "}");
  //strcat(msg, '\0');

  String strMsg = String("{\"analog\":");
  strMsg += String(analogRead(0));
  strMsg += String( "}" );

	Serial.print("publishing message: ");
	Serial.println(strMsg);
	client = PubNub.publish(channel, strMsg.c_str());
	if (!client) {
		Serial.println("publishing error");
	} else {
		flash(pubLedPin);
		client->stop();
	}

	delay(5000);
}
