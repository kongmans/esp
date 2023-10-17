/**********************************************************************
项目名称/Project          : 零基础入门学用物联网
程序名称/Program name     : b_2_wifimanger_example
团队/Team                : 太极创客团队 / Taichi-Maker (www.taichi-maker.com)
作者/Author              : CYNO朔
日期/Date（YYYYMMDD）     : 2020703
程序目的/Purpose          :
测试WiFiManager库所配置的ESP8266的确可以连接到互联网，并从互联网上的网络服务器获取信息。
如需获取本示例程序的详细说明，请前往以下链接：
http://www.taichi-maker.com/homepage/esp8266-nodemcu-iot/iot-c/esp8266-tips/wifimanager/
-----------------------------------------------------------------------
其它说明 / Other Description：
本程序为太极创客团队制作的免费视频教程《零基础入门学用物联网 》中一部分。该教程系统的
向您讲述ESP8266的物联网应用相关的软件和硬件知识。以下是该教程目录页：
http://www.taichi-maker.com/homepage/esp8266-nodemcu-iot/     
***********************************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
         
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 
 
// 测试HTTP请求用的URL。注意网址前面必须添加"http://"
#define URL "http://www.example.com"
 
void setup() {
  //初始化串口设置
  Serial.begin(9600);
 
  // 建立WiFiManager对象
  WiFiManager wifiManager;
  
  // 自动连接WiFi。以下语句的参数是连接ESP8266时的WiFi名称
  wifiManager.autoConnect("AutoConnectAP","123456");
  
  Serial.print("WiFi Connected!");
  
  httpClientRequest();  
}
 
void loop() {}
 
// 发送HTTP请求并且将服务器响应通过串口输出
void httpClientRequest(){
 
  //重点1 创建 HTTPClient 对象
  HTTPClient httpClient;
 
  //重点2 通过begin函数配置请求地址。此处也可以不使用端口号和PATH而单纯的
  httpClient.begin(URL); 
  Serial.print("URL: "); Serial.println(URL);
 
  //重点3 通过GET函数启动连接并发送HTTP请求
  int httpCode = httpClient.GET();
  Serial.print("Send GET request to URL: ");
  Serial.println(URL);
  
  //重点4. 如果服务器响应HTTP_CODE_OK(200)则从服务器获取响应体信息并通过串口输出
  //如果服务器不响应HTTP_CODE_OK(200)则将服务器响应状态码通过串口输出
  if (httpCode == HTTP_CODE_OK) {
    // 使用getString函数获取服务器响应体内容
    String responsePayload = httpClient.getString();
    Serial.println("Server Response Payload: ");
    Serial.println(responsePayload);
  } else {
    Serial.println("Server Respose Code：");
    Serial.println(httpCode);
  }
 
  //重点5. 关闭ESP8266与服务器连接
  httpClient.end();
}
