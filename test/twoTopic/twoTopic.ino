/*
* Time:20200508
* Author: youngda
* QQ:1217882800
* wechat:19092550573
* QQ群：824273231
* 项目示例：通过发送on或off控制开关
*/

#include <ESP8266WiFi.h>//默认，加载WIFI头文件
#include "PubSubClient.h"//默认，加载MQTT库文件

const char* ssid = "布凡";//修改，你的路由去WIFI名字
const char* password = "20202020";//你的WIFI密码
const char* mqtt_server = "bemfa.com";//默认，MQTT服务器
const int mqtt_server_port = 9501;//默认，MQTT服务器
#define ID_MQTT  "9b6d3f9823f74f90ac83c424cf8ae2b1"     //修改，你的Client ID，9b6d3f9823f74f90ac83c424cf8ae2b1

const char*  topicA = "82660a002";  //主题名字，可在巴法云控制台自行创建，名称随意
const char*  topicB = "82660b002";  //主题名字，可在巴法云控制台自行创建，名称随意

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//灯光函数及引脚定义
void turnOnLed();
void turnOffLed();
const int B_led = D4;


void setup_wifi() {
  delay(10);
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
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String Mqtt_Buff = "";
  for (int i = 0; i < length; i++) {
    Mqtt_Buff += (char)payload[i];
  }
  Serial.print(Mqtt_Buff);
  Serial.println();

  if(strcmp(topic,topicA)==0){//判断topic是否是topicA
       Serial.println(" topicA msg");
        if(Mqtt_Buff == "on") {//如果接收字符on，亮灯
           turnOnLed();//开灯函数
        } else if (Mqtt_Buff == "off") {//如果接收字符off，亮灯
           turnOffLed();//关灯函数
        }
  }else if(strcmp(topic,topicB)==0){//判断topic是否是topicB
       Serial.println(" topicB msg");
      if (Mqtt_Buff == "on") {//如果接收字符on，亮灯
          turnOnLed();//开灯函数
      } else if (Mqtt_Buff == "off") {//如果接收字符off，亮灯
          turnOffLed();//关灯函数
      }
  }

  Mqtt_Buff = "";
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID_MQTT)) {
      Serial.println("connected");

      client.subscribe(topicA);//修改，修改为你的主题
      delay(100);
      client.subscribe(topicB);//修改，修改为你的主题
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  pinMode(B_led, OUTPUT);
  digitalWrite(B_led, HIGH);
   
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_server_port);
  client.setCallback(callback);
 
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}

//打开灯泡
void turnOnLed() {
  Serial.println("turn on light");
  digitalWrite(B_led, LOW);
}
//关闭灯泡
void turnOffLed() {
    Serial.println("turn off light");
  digitalWrite(B_led, HIGH);
}
