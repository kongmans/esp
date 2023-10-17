/**********************************************************************
项目名称/Project          : 零基础入门学用物联网
程序名称/Program name     : c_parameter
团队/Team                : 太极创客团队 / Taichi-Maker (www.taichi-maker.com)
作者/Author              : CYNO朔
日期/Date（YYYYMMDD）     : 20200703
程序目的/Purpose          : 
本程序旨在演示如何向Ticker库定时调用函数来传递参数。
请注意：attach函数所能传递的参数最多只有一个。
另外该参数仅能是这些类型中的一种：char, short, int, float, void*, char*
 
如需了解本程序的详细说明，请参考以下函数：
http://www.taichi-maker.com/homepage/esp8266-nodemcu-iot/iot-c/esp8266-tips/tinker/
-----------------------------------------------------------------------
其它说明 / Other Description：
本程序为太极创客团队制作的免费视频教程《零基础入门学用物联网 》中一部分。该教程系统的
向您讲述ESP8266的物联网应用相关的软件和硬件知识。以下是该教程目录页：
http://www.taichi-maker.com/homepage/esp8266-nodemcu-iot/                    
***********************************************************************/
#include <Ticker.h>
 
Ticker ticker;
 
int count;
 
void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  ticker.attach(1, sayHi, 8);
}
 
void loop() {
  for (int fadeValue = 0 ; fadeValue <= 1023; fadeValue += 5) {
    analogWrite(LED_BUILTIN, fadeValue);
    delay(10);
  }
 
  for (int fadeValue = 1023 ; fadeValue >= 0; fadeValue -= 5) {
    analogWrite(LED_BUILTIN, fadeValue);
    delay(10);
  }
  delay(3000);
}
 
void sayHi(int hiTimes){
  count++;
  Serial.print("Hi ");
  Serial.println(count);
 
  if (count >= hiTimes) {
    ticker.detach();
    Serial.print("ticker.detach();");
  }
}
