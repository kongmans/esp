#include <WiFiManager.h>
#include <strings_en.h>
#include <wm_consts_en.h>
#include <wm_strings_en.h>
#include <wm_strings_es.h>

/**********************************************************************
项目名称/Project          : 零基础入门学用物联网
程序名称/Program name     : a_basic
团队/Team                : 太极创客团队 / Taichi-Maker (www.taichi-maker.com)
作者/Author              : CYNO朔
日期/Date（YYYYMMDD）     : 20200703
程序目的/Purpose          : 
利用WiFiManager库实现最基本的WiFi配置工作
如需获取本示例程序的详细说明，请前往以下链接：
http://www.taichi-maker.com/homepage/esp8266-nodemcu-iot/iot-c/esp8266-tips/wifimanager/
-----------------------------------------------------------------------
其它说明 / Other Description：
本程序为太极创客团队制作的免费视频教程《零基础入门学用物联网 》中一部分。该教程系统的
向您讲述ESP8266的物联网应用相关的软件和硬件知识。以下是该教程目录页：
http://www.taichi-maker.com/homepage/esp8266-nodemcu-iot/                    
***********************************************************************/
#include <ESP8266WiFi.h>          
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         
 
void setup() {
    Serial.begin(9600);       
    // 建立WiFiManager对象
    WiFiManager wifiManager;
    
    // 自动连接WiFi。以下语句的参数是连接ESP8266时的WiFi名称
    //wifiManager.autoConnect("AutoConnectAP");
    
    // 如果您希望该WiFi添加密码，可以使用以下语句：
     wifiManager.autoConnect("AutoConnectAP", "12345678");
    // 以上语句中的12345678是连接AutoConnectAP的密码
    
    // WiFi连接成功后将通过串口监视器输出连接成功信息 
    Serial.println(""); 
    Serial.print("ESP8266 Connected to ");
    Serial.println(WiFi.SSID());              // WiFi名称
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());           // IP
}
 
void loop() {}
