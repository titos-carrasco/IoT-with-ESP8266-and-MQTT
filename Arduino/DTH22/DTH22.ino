#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient wiFiClient;
PubSubClient mqtt( wiFiClient );

#define WIFI_SSID       "wifi ssid"
#define WIFI_PASS       "wifi pass"

#define MQTT_SERVER     "mqtt.mosquitto.org"                
#define MQTT_PORT       1883
String  MQTT_CLIENTID = "Node_" + String( ESP.getChipId() );

//***********************************
#include "DHT.h"

DHT dht( D2, DHT22 );

//***********************************

void setup()
{
  Serial.begin( 115200 );
  Serial.println();
  Serial.println();

  mqtt.setServer( MQTT_SERVER, MQTT_PORT );

  dht.begin();
  delay(1000);
}

void loop()
{
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();
  yield();

  Serial.print( "Temperatura: " );
  Serial.print( temperatura );
  Serial.print( "°C" );
  Serial.print( ", Humedad:" );
  Serial.print( humedad );
  Serial.print( "%" );
  Serial.println();
  yield();

  //delay( 1000 );
  //return;
  
  WiFi.mode( WIFI_STA );
  WiFi.begin( WIFI_SSID, WIFI_PASS );
  Serial.print( "Conectando a la WiFi ." );
  unsigned long start = millis();
  while( ( millis() - start ) < 10UL * 1000UL ){
    Serial.print( "." );
    if(  WiFi.status() == WL_CONNECTED ){
      Serial.println( " : Conectado." );
      Serial.println ( "Conectando a servidor MQTT" );
      if( mqtt.connect( MQTT_CLIENTID.c_str() ) ){
        Serial.println( "Publicando ..." );
        char s[32];
        dtostrf( temperatura, 7, 2, s );
        mqtt.publish( "rcr/dht22/01/temperatura", s );
        dtostrf( humedad, 7, 2, s );
        mqtt.publish( "rcr/dht22/01/humedad", s );
      }
      break;
    } else {
      delay( 500 );
    }
  }
  Serial.println();
  Serial.println( "Reiniciando ..." );

  ESP.deepSleep(30UL * 1000000UL);
}
