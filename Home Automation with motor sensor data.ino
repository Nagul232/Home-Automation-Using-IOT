#include <ESP8266WiFi.h>
#include "config.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo.h>
#define WIFI_SSID "nana"
#define WIFI_PASS "NagullugaN"

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "nagul"
#define MQTT_PASS "457843b8b787487ca56b7e2bc877b9f6"
int count = 0;


AdafruitIO_Feed *counter = io.feed("counter");
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe openclose = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/openclose");
Adafruit_MQTT_Publish servoStatus = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/servoStatus");
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/temperature");
Servo Servo1;
float tempvalue;
int rainvalue;
void setup()
{
  Serial.begin(9600);
  //pinMode(LED_BUILTIN, OUTPUT);


  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi>");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  //digitalWrite(LED_BUILTIN, LOW);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(">");
    delay(50);
  }
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  Serial.println("OK!");

  //Subscribe to the onoff topic
  mqtt.subscribe(&openclose);
 pinMode(D6,INPUT);
 pinMode(A0,INPUT);
 Servo1.attach(D7);
  //digitalWrite(LED_BUILTIN, HIGH);
  Servo1.write(0);

}

void loop()
{
  tempvalue = analogRead(A0);
  tempvalue = (tempvalue*500)/1023;
  rainvalue = digitalRead(D6);
  //Connect/Reconnect to MQTT
  MQTT_connect();

  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &openclose)
    {
      //Print the new value to the serial monitor
      Serial.print("openclose: ");
      Serial.println((char*) openclose.lastread);

      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) openclose.lastread, "open"))
      {
        //active low logic
       Servo1.write(90);
        servoStatus.publish("open");
      }
      else if (!strcmp((char*) openclose.lastread, "close"))
      {
        Servo1.write(45);
        servoStatus.publish("close");

      }
      else
      {
        servoStatus.publish("ERROR");
      }
    }
    else
    {
      //LightsStatus.publish("ERROR");
    }
  }
  //  if (!mqtt.ping())
  //  {
  //    mqtt.disconnect();
  //  }
}


void MQTT_connect()
{

  //  // Stop if already connected
  if (mqtt.connected() && mqtt.ping())
  {
    //    mqtt.disconnect();
    return;
  }

  int8_t ret;

  mqtt.disconnect();

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      ESP.reset();
    }
  }
  Serial.println("MQTT Connected!");
}
