/**********************************************************************
项目名称/Project          : 零基础入门学用物联网
程序名称/Program name     : e_timer_http
团队/Team                : 太极创客团队 / Taichi-Maker (www.taichi-maker.com)
作者/Author              : CYNO朔
日期/Date（YYYYMMDD）     : 20200703
程序目的/Purpose          : 
本程序旨在演示如何使用计数器来控制ESP8266定时执行较复杂的函数。Ticker定时调用的函数必须要“短小精悍”。
而不能是复杂且占用时间较长的函数。对于较为复杂的函数，我们可以使用计数器的方法来实现。
 
本程序将会定时让ESP8266向example网络服务器发送请求，并且将服务器响应信息显示在屏幕中。
 
如需了解本程序的详细说明，请参考以下函数：
http://www.taichi-maker.com/homepage/esp8266-nodemcu-iot/iot-c/esp8266-tips/tinker/
-----------------------------------------------------------------------
其它说明 / Other Description：
本程序为太极创客团队制作的免费视频教程《零基础入门学用物联网 》中一部分。该教程系统的
向您讲述ESP8266的物联网应用相关的软件和硬件知识。以下是该教程目录页：
http://www.taichi-maker.com/homepage/esp8266-nodemcu-iot/                    
***********************************************************************/
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
 
#define URL "http://www.example.com"
 
// 设置wifi接入信息(请根据您的WiFi信息进行修改)
const char* ssid = "布凡";
const char* password = "20202020";
 
Ticker ticker;
 
int count;
 
void setup() {
  Serial.begin(9600);
 
  //设置ESP8266工作模式为无线终端模式
  WiFi.mode(WIFI_STA);
  
  //连接WiFi
  connectWifi();
 
  ticker.attach(1, tickerCount);
}
 
void loop() {  
  if (count >= 5){   
    httpRequest();
    count = 0;
  }
}
 
void tickerCount(){
  count++;
  Serial.print("count = ");
  Serial.println(count);
}
 
// 发送HTTP请求并且将服务器响应通过串口输出
void httpRequest(){
  HTTPClient httpClient;
 
  httpClient.begin(URL); 
  Serial.print("URL: "); Serial.println(URL);
 
  int httpCode = httpClient.GET();
  Serial.print("Send GET request to URL: ");
  Serial.println(URL);
  
  if (httpCode == HTTP_CODE_OK) {
    // 使用getString函数获取服务器响应体内容
    String responsePayload = httpClient.getString();
    Serial.println("Server Response Payload: ");
    Serial.println(responsePayload);
  } else {
    Serial.println("Server Respose Code：");
    Serial.println(httpCode);
  }
  httpClient.end();
}
 
void connectWifi(){
//开始连接wifi
  WiFi.begin(ssid, password);
 
  //等待WiFi连接,连接成功打印IP
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi Connected!");   
}
