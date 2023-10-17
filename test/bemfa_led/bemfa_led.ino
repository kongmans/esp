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

#define server_ip "bemfa.com"   //巴法云服务器地址默认即可
#define server_port "8344"   //服务器端口，tcp创客云端口8344

//********************需要修改的部分*******************//
String UID = "4d9ec352e0376f2110a0c601a2857225"; //用户私钥，可在控制台获取,修改为自己的UID
String TOPIC = "led002";  //主题名字，可在控制台新建
const int led_Pin = D2;  //需要控制的led引脚
//****************************************************//




//tcp客户端相关初始化，默认即可
#define MAX_PACKETSIZE 512  //最大字节数
#define KEEPALIVEATIME 30*1000  //设置心跳值30s
WiFiClient TCPclient;
String TcpClient_Buff = "";
unsigned int TcpClient_BuffIndex = 0;
unsigned long TcpClient_preTick = 0;
unsigned long preHeartTick = 0;//心跳
unsigned long preTCPStartTick = 0;//连接
bool preTCPConnected = false;

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

//相关函数初始化
void doWiFiTick();       //检查WIFI状态
void doTCPClientTick();  //进行tcp连接相关
void startTCPClient();   //开始连接tcp
void sendtoTCPServer(String p); //tcp发送数据

config_type config;
void doSmartconfig();
void saveConfig();
void initWiFi();
void loadConfig();
void restoreFactory();
void waitKey();


//led控制函数，具体函数内容见下方
void turnOnLed();
void turnOffLed();



/*
  *发送数据到TCP服务器
 */
void sendtoTCPServer(String p){
  if (!TCPclient.connected()) 
  {
    Serial.println("Client is not readly");
    return;
  }
  TCPclient.print(p);
  preHeartTick = millis();//心跳计时开始，需要每隔60秒发送一次数据
}


/*
  *初始化和服务器建立连接
*/
void startTCPClient(){
  if(TCPclient.connect(server_ip, atoi(server_port))){
    Serial.print("\nConnected to server:");
    Serial.printf("%s:%d\r\n",server_ip,atoi(server_port));
    
    String tcpTemp="";  //初始化字符串
    tcpTemp = "cmd=1&uid="+UID+"&topic="+TOPIC+"\r\n"; //构建订阅指令
    sendtoTCPServer(tcpTemp); //发送订阅指令
    tcpTemp="";//清空
    /*
     //如果需要订阅多个主题，可发送  cmd=1&uid=xxxxxxxxxxxxxxxxxxxxxxx&topic=xxx1,xxx2,xxx3,xxx4\r\n
    教程：https://bbs.bemfa.com/64
     */
    
    preTCPConnected = true;
    TCPclient.setNoDelay(true);
  }
  else{
    Serial.print("Failed connected to server:");
    Serial.println(server_ip);
    TCPclient.stop();
    preTCPConnected = false;
  }
  preTCPStartTick = millis();
}


/*
  *检查数据，发送心跳
*/
void doTCPClientTick(){
 //检查是否断开，断开后重连
   if(WiFi.status() != WL_CONNECTED) return;
  if (!TCPclient.connected()) {//断开重连
  if(preTCPConnected == true){
    preTCPConnected = false;
    preTCPStartTick = millis();
    Serial.println();
    Serial.println("TCP Client disconnected.");
    TCPclient.stop();
  }
  else if(millis() - preTCPStartTick > 1*1000)//重新连接
    startTCPClient();
  }
  else
  {
    if (TCPclient.available()) {//收数据
      char c =TCPclient.read();
      TcpClient_Buff +=c;
      TcpClient_BuffIndex++;
      TcpClient_preTick = millis();
      
      if(TcpClient_BuffIndex>=MAX_PACKETSIZE - 1){
        TcpClient_BuffIndex = MAX_PACKETSIZE-2;
        TcpClient_preTick = TcpClient_preTick - 200;
      }
 
    }
    if(millis() - preHeartTick >= KEEPALIVEATIME){//保持心跳
      preHeartTick = millis();
      Serial.println("--Keep alive:");
      sendtoTCPServer("ping\r\n"); //发送心跳，指令需\r\n结尾，详见接入文档介绍
    }
  }
  if((TcpClient_Buff.length() >= 1) && (millis() - TcpClient_preTick>=200))
  {
    TCPclient.flush();
    Serial.print("Rev string: ");
    TcpClient_Buff.trim(); //去掉首位空格
    Serial.println(TcpClient_Buff); //打印接收到的消息
    String getTopic = "";
    String getMsg = "";
    if(TcpClient_Buff.length() > 15){//注意TcpClient_Buff只是个字符串，在上面开头做了初始化 String TcpClient_Buff = "";
          //此时会收到推送的指令，指令大概为 cmd=2&uid=xxx&topic=light002&msg=off
          int topicIndex = TcpClient_Buff.indexOf("&topic=")+7; //c语言字符串查找，查找&topic=位置，并移动7位，不懂的可百度c语言字符串查找
          int msgIndex = TcpClient_Buff.indexOf("&msg=");//c语言字符串查找，查找&msg=位置
          getTopic = TcpClient_Buff.substring(topicIndex,msgIndex);//c语言字符串截取，截取到topic,不懂的可百度c语言字符串截取
          getMsg = TcpClient_Buff.substring(msgIndex+5);//c语言字符串截取，截取到消息
          Serial.print("topic:------");
          Serial.println(getTopic); //打印截取到的主题值
          Serial.print("msg:--------");
          Serial.println(getMsg);   //打印截取到的消息值
   }
   if(getMsg  == "on"){       //如果是消息==打开
     turnOnLed();
   }else if(getMsg == "off"){ //如果是消息==关闭
      turnOffLed();
    }

   TcpClient_Buff="";
   TcpClient_BuffIndex = 0;
  }
}



/**************************************************************************
                                 WIFI
***************************************************************************/
/*
  WiFiTick
  检查是否需要初始化WiFi
  检查WiFi是否连接上，若连接成功启动TCP Client
  控制指示灯
*/
void doWiFiTick(){
  static bool startSTAFlag = false;
  static bool taskStarted = false;
  static uint32_t lastWiFiCheckTick = 0;

  if (!startSTAFlag) {
    startSTAFlag = true;
    initWiFi();
  }

  //未连接1s重连
  if ( WiFi.status() != WL_CONNECTED ) {
    if (millis() - lastWiFiCheckTick > 1000) {
      lastWiFiCheckTick = millis();
    }
  }
  //连接成功建立
  else {
    if (taskStarted == false) {
      taskStarted = true;
      Serial.print("\r\nGet IP Address: ");
      Serial.println(WiFi.localIP());
      startTCPClient();
    }
  }
}
//打开灯泡
void turnOnLed(){
  Serial.println("Turn ON");
  digitalWrite(led_Pin,LOW);
}
//关闭灯泡
void turnOffLed(){
  Serial.println("Turn OFF");
  digitalWrite(led_Pin,HIGH);
}




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
  doWiFiTick();//检查wifi是否正常
  doTCPClientTick();//保持tcp连接，并接收数据
}
