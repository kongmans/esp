#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);



void setup() {
  // put your setup code here, to run once:
  dht.begin();
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  float h=dht.readHumidity();
  float t=dht.readTemperature();


  Serial.print("当前湿度：");
  Serial.print(h);
  Serial.println("%");

  Serial.print("当前温度：");
  Serial.print(t);
  Serial.println("℃");
  
}
