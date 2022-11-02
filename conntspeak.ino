
#include <SFE_BMP180.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include "DHT.h"

DHT dht(D3, DHT11);
SFE_BMP180 bmp;
int threshold = 13;
double T, P;
char status;
WiFiClient client;

String apiKey = "KLJZDR0JD0DDADAW";
const char *ssid =  "Sai Ram";
const char *pass =  "Kaminari@17";
const char* server = "api.thingspeak.com";


void setup() {
  Serial.begin(115200);
  delay(10);
  bmp.begin();
  Wire.begin();
  dht.begin();
  pinMode(threshold,INPUT_PULLUP);
  WiFi.begin(ssid, pass);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  //BMP180 sensor
  status =  bmp.startTemperature();
  if (status != 0) {
    delay(status);
    status = bmp.getTemperature(T);

    status = bmp.startPressure(3);// 0 to 3
    if (status != 0) {
      delay(status);
      status = bmp.getPressure(P, T);
      if (status != 0) {

      }
    }
  }

  //DHT11 sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //Gas sensor
  float gas = analogRead(A0);
  if (isnan(gas)){
    Serial.println("Failed to read MQt Sensor!");
    return;
  }

  //Rain sensor
  int r = digitalRead(threshold);
  if (isnan(r)){
    Serial.println("Failed to read Rain Sensor!");
    return;
  }

  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "&field3=";
    postStr += String(P, 2);
    postStr += "&field4=";
    postStr += String(r);
    postStr += "&field5=";
    postStr += String(gas);
    postStr += "\r\n\r\n\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.println(t);
    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print("Absolute Pressure: ");
      Serial.print(P, 2);
    Serial.println("mb");
    Serial.print("Rain: ");
    Serial.println(r);
    Serial.print("Gas: ");
    Serial.println(gas);

  }
  client.stop();
  delay(1000);
}
