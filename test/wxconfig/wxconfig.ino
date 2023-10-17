/**
* Time:2022-08-07
* Author: 巴法云技术部
* QQ群：439722037
* 微信内搜索 “一键配网” 小程序进行配网
* 或者可使用巴法app进行配网，app下载地址：https://app.bemfa.com
* 
* 配网后重新配网：ESP8266连续按5次reset 按键，必须连续按，10s内完成。或者断电重启5次，会重置配网信息，
                  需要重新配网，即开五次关五次，每次开关间隔不超过2秒。或者在想要恢复出厂设置的地方调用函数restoreFactory();会清除配网信息。
*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <Ticker.h>
#define HOST_NAME  "bemfa"
char config_flag = 0;
#define MAGIC_NUMBER 0xAA

/**
* 结构体，用于存储配网信息
*/
struct config_type
{
  char stassid[32];
  char stapsw[64];
   char cuid[40];
   char ctopic[32];
  uint8_t reboot;
  uint8_t magic;
};
config_type config;
void doSmartconfig();
void saveConfig();
void initWiFi();
void loadConfig();
void restoreFactory();
void waitKey();

/**
* airkiss配网
*/
void doSmartconfig(){
  Serial.print(" Smartconfig begin,Waiting for WeChat Config.....");
  WiFi.mode(WIFI_STA);
  WiFi.stopSmartConfig();
  WiFi.beginSmartConfig();
  int cnt = 0;
  bool flag_ok = false;
  while (!WiFi.smartConfigDone()) {
    delay(300);
    if (flag_ok == true) continue;
    if (WiFi.smartConfigDone()) {
        strcpy(config.stassid, WiFi.SSID().c_str());
        strcpy(config.stapsw, WiFi.psk().c_str());
        config.magic = 0xAA;
        saveConfig();
        flag_ok = true;
        
    }
    cnt++;
    if (cnt >= 600) {
      delayRestart(0);
    }
  }
}

/**
* 存储配网信息
*/
void saveConfig(){
 int rand_key;
 uint8_t mac[6];
 WiFi.macAddress(mac);
 config.reboot = 0;
 EEPROM.begin(512);
 uint8_t *p = (uint8_t*)(&config);
 for (int i = 0; i < sizeof(config); i++)
 {
   EEPROM.write(i, *(p + i));
 }
 EEPROM.commit();
 }
 Ticker delayTimer;
 void delayRestart(float t) {
 delayTimer.attach(t, []() {
 ESP.restart();
 });
}

/**
* 初始化wifi信息，并连接路由器网络
*/
void initWiFi(){
  char temp[32];
  uint8_t mac[6];
  WiFi.macAddress(mac);
  sprintf(temp, "%s_%02X%02X%02X", HOST_NAME, mac[3], mac[4], mac[5]);
  WiFi.hostname(temp);
  if(WiFi.status() != WL_CONNECTED){
    WiFi.disconnect();//断开连接
    WiFi.mode(WIFI_STA);//STA模式
    WiFi.begin(config.stassid, config.stapsw);//连接路由器
  }
 while (WiFi.status() != WL_CONNECTED) {//检查是否连接成功
  delay(500);
   Serial.print(".");
 }
   if(config_flag == 1){
      setConfig();
    }
  Serial.println("wifi config ok");
  WiFi.softAP(temp);
}
/**
* 设置SmartConfig
*/
void setConfig(){
    String mac = WiFi.macAddress().substring(8);//取mac地址做主题用
    mac.replace(":", "");//去掉:号
    WiFiClient client_bemfa_WiFiClient;
    HTTPClient http_bemfa_HTTPClient;
    http_bemfa_HTTPClient.begin(client_bemfa_WiFiClient,"http://pro.bemfa.com/vv/setSmartConfig?version=1&user="+mac);
    int httpCode = http_bemfa_HTTPClient.GET();
    if (httpCode == 200) {
      Serial.println("wifi smartconfig ok");
    }
    http_bemfa_HTTPClient.end();
}

/**
* 加载存储的信息，并检查是否进行了反复5次重启恢复出厂信息
*/
uint8_t *p = (uint8_t*)(&config);
void loadConfig()
{
    uint8_t mac[6];
    WiFi.macAddress(mac);
    EEPROM.begin(512);
    for (int i = 0; i < sizeof(config); i++)
      {
      *(p + i) = EEPROM.read(i);
    }
    config.reboot = config.reboot + 1;
    if(config.reboot>=4){
        restoreFactory();
    }
    if(config.magic != 0xAA){
      config_flag = 1;
    }
    EEPROM.begin(512);
    for (int i = 0; i < sizeof(config); i++){
      EEPROM.write(i, *(p + i));
    }
      EEPROM.commit();
    delay(2000);
    EEPROM.begin(512);
    config.reboot = 0;
    for (int i = 0; i < sizeof(config); i++){
      EEPROM.write(i, *(p + i));
    }
    EEPROM.commit();
    delay(2000);
}
/**
* 恢复出厂设置，清除存储的wifi信息
*/
void restoreFactory(){
    Serial.println("Restore Factory....... ");
    config.magic = 0x00;
    strcpy(config.stassid, "");
    strcpy(config.stapsw, "");
    config.magic = 0x00;
    saveConfig();
    delayRestart(1);
    while (1) {
        delay(100);
    }
 }
/**
* 检查是否需要airkiss配网
*/
void waitKey()
{
  if(config_flag == 1){
      doSmartconfig();
    }
}

/**
* 单片机初始化函数
*/
void setup(){
  Serial.begin(115200);
  loadConfig();
  waitKey();
  initWiFi();
}

void loop(){
  /*测试完毕后，删掉以下无用的两行*/
  Serial.println("config ok");
  delay(1000);
}
