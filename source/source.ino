/////////////////////////////////////////////////////////////////
//扩展模块分布
//NC//SCL          PIN4
//数字信号输入/SDA   PIN3
//VCC              PIN2
//GND              PIN1

//A1              PIN4
//A0              PIN3
//VCC             PIN2
//GND             PIN1


//
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//Serial START

/////////////////////////
//Heartrate START

#define PIN A0  // 心率模块使用的端口
unsigned long time0;
unsigned long time_cache0;
bool sta = false, sta_cache = true;
unsigned long getTime0() {
  //返回   从按下的时间开始-------到松手的时间
  sta_cache = sta;
  sta = digitalRead(PIN);
  //   Serial.print("sta = ");
  // Serial.print(sta);
  //     Serial.print("sta_cache = ");
  // Serial.print(sta_cache);
  //   Serial.println();
  delayMicroseconds(200);
  if (sta == HIGH && sta_cache == LOW) {
    time_cache0 = time0;
    time0 = millis();
    //     Serial.print("本次有效");
    return (time0 - time_cache0);
  }
  else {
    //     Serial.print("本次无效");
    //   Serial.println();
    return 0;
  }
}

#define time1_MAX 200
#define time1_MIN 50  //过快的波

unsigned long time1;
unsigned long time1_cache;
boolean time1_sta_error = false;
unsigned long getTime1(unsigned long _t) {
  time1_cache = _t;
  if (time1_cache) {  //有数据时才执行滤波
    if (time1_cache < time1_MAX) {   //滤最小阈值
      if (time1_cache < time1_MIN) { //若有过快的波，开启标志位
        //       Serial.println("make");
        time1_sta_error = true;
      }
      return 0;
    }
    else {
      //      Serial.print(".");
      if (time1_sta_error) {  //如果被标志 则本次波形无效
        //        Serial.println("make done");
        time1_sta_error = false;
        return 0;
      }
      return time1_cache;
    }
  }
  else {
    return 0;
  }
}

#define pulse_MIN 40          //t < 1500ms
#define pulse_MAX 150         //t > 400ms
#define pulse_UPDATA  3
int pulse_num = 0;
unsigned long pulse_vol = 0;
unsigned long pulse_cache = 0;
unsigned long pulse(unsigned long _t) {       //_t 表示按压时间
  if (!_t)
    return 0;

  uint32_t  _pulse_vol = 60000 / _t;

  if ((_pulse_vol > pulse_MIN) && (_pulse_vol < pulse_MAX)) {
    pulse_vol += _pulse_vol;
    pulse_num++;
    if (pulse_num > (pulse_UPDATA - 1)) {
      uint32_t _pulse = pulse_vol / pulse_UPDATA;
      pulse_num = 0;
      pulse_vol = 0;
      pulse_cache = _pulse;
      return _pulse;
    }
    else {
      return 0;
    }
  }
  else {
    pulse_num = 0;
    pulse_vol = 0;
    return 0;
  }
}

int heartrate = 0;
void getheartrate()
{
    //////////////////
  //心率
  long presstime = getTime0();   //按压时间
  long islegal = getTime1(presstime); //检测有效性    有效返回按压时间 无效返回0
  long retpulse = pulse(islegal);    //心率

  if (presstime) {
    if (islegal) {
      if (retpulse) {
      heartrate = retpulse;
      Serial.print("Current heartrate：");
      Serial.println(heartrate);
      }
    }
  }
}

//Heartrate END
///////////////////////////////////


///////////////////////////////////
//Temperature START

#include <Microduino_Tem_Hum.h>
Tem_D1  termo;  //调用Sensor-Temperature-D1传感器

int temperature = 0;
void gettemperature()
{
  ///////////////////
  //温度
  //时间控制 10秒左右测一次   
  //static float timeflag = 0;
  //static float nowtime = 0;
  //timeflag = millis() - nowtime;  
  if (!termo.begin())
  {
    //串口打印传感器不在线
    Serial.println("Termo bad");
  }
  //if (timeflag >= 10000) {
    //nowtime = millis();
    temperature = termo.getTemperature();
    Serial.print("Current temperature: ");
    Serial.print(temperature);
    Serial.println(" C");
  //}
}

//Temperature END
//////////////////////////////////


//////////////////////////////////
//Weight START

// Hx711.DOUT - pin #A1
// Hx711.SCK - pin #A0

#include <Hx711.h>

//Hx711 scale(A1, A0);
Hx711 scale(A7, A6);

int weight = 0;
void getweight()
{

  ///////////////////
  //体重
  //时间控制 10秒左右测一次   
  //static float timeflag = 0;
  //static float nowtime = 0;
  //timeflag = millis() - nowtime;  
  //if (timeflag >= 10000) {
    //nowtime = millis();
    weight = scale.getGram();
    Serial.print("Current weight: ");
    Serial.print(weight);
    Serial.println(" g");
  //}
}

//Weight END
//////////////////////////////////

//////////////////////////////////
//gas Sensor MQ-3 START

int gas_din=2;
int gas_ain=A2;
int alcohol;

void getalcohol()
{
  alcohol = analogRead(gas_ain);
  if(digitalRead(gas_din)==LOW)
  {
    Serial.println("Gas leakage：");
    Serial.println(alcohol);
  }
  else
  {
    Serial.println("Gas not leak");
  }
}


//gas Sensor MQ-3 START
//////////////////////////////////


/////////////////////////////////
//ALL START
////获取传感器数据
void getSensorData()
{

  getheartrate();   //获取心率
  gettemperature(); //获得温度
  getweight();      //获取体重
  getalcohol();     //获取酒精浓度
  gettime();        //获取时间

}
//ALL END
/////////////////////////////////

//Serial END
/////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////
//WIFI START

#include <ESP8266.h>    //WIFI

/**
**CoreUSB UART Port: [Serial1] [D0,D1]
**Core+ UART Port: [Serial1] [D2,D3]
**/
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1284P__) || defined (__AVR_ATmega644P__) || defined(__AVR_ATmega128RFA1__)
#define EspSerial Serial1
#define UARTSPEED  115200
#endif

/**
**Core UART Port: [SoftSerial] [D2,D3]
**/
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); /* RX:D2, TX:D3 */

#define EspSerial mySerial
#define UARTSPEED  19200
#endif

#define SSID_NAME   "xxx"
#define PASSWORD    "xxx"

#define HOST_NAME   "127.0.0.1"
#define HOST_PORT   9527


ESP8266 wifi(&EspSerial);
bool connectflag = false; 
void wifiInit()
{
  WifiInit(EspSerial, UARTSPEED);
  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());
  if (wifi.setOprToStationSoftAP()) {
    Serial.print("to station + softap ok\r\n");
  } else {
    Serial.print("to station + softap err\r\n");
  }
  if (wifi.joinAP(SSID_NAME, PASSWORD)) {
    Serial.print("Join AP success\r\n");
    Serial.print("IP:");
    Serial.println( wifi.getLocalIP().c_str());
    connectflag = true;
  }
  else {
    Serial.print("Join AP failure\r\n");
  }
  if (wifi.disableMUX()) {
      Serial.print("signal ok\r\n");
  } else {
    Serial.print("signal ok\r\n");
  }
  Serial.print("setup end\r\n");
}


/////////////////////////
//Time START


#define TIME_HOST_NAME   "pool.ntp.org"
#define TIME_HOST_PORT   (123)
#define TIME_ZONE +8
/*==============================================================================*/
/* Useful Constants */
#define SECS_PER_HOUR (3600UL)

#include <TimeLib.h>

uint8_t buffer[128] = {0};
uint32_t len = 0;
int Year, Month, Day, Hour, Minute, Second, Weekday;
time_t prevDisplay = 0;

void updateTimeData() {
  do {
    delay(200);
    registerUDPAndSendRecvData();
    if (len > 0) {
      getTimeStampAndSetRTC();
      unregisterUDP();
    } else {
      unregisterUDP();
    }
  } while (!len);
}

void getTimeStampAndSetRTC() {
  Serial.print("Received:[");
  unsigned long t = (((unsigned long)buffer[40] << 24) |
                     ((unsigned long)buffer[41] << 16) |
                     ((unsigned long)buffer[42] <<  8) |
                     (unsigned long)buffer[43]) - 2208988800UL;

  Serial.print("Unix timestamp:");
  Serial.print(t);
  Serial.print("]\r\n");

  setTime(t);
  adjustTime(TIME_ZONE * SECS_PER_HOUR);
}

void registerUDPAndSendRecvData() {
  if (wifi.registerUDP(TIME_HOST_NAME, TIME_HOST_PORT)) {
    Serial.print("register udp ");
    Serial.println(" ok");
  } else {
    Serial.print("register udp ");
    Serial.println(" err");
  }

  static const char PROGMEM
  timeReqA[] = { 227,  0,  6, 236 }, timeReqB[] = {  49, 78, 49,  52 };
  // Assemble and issue request packet
  uint8_t       buf[48];
  memset(buf, 0, sizeof(buf));
  memcpy_P( buf    , timeReqA, sizeof(timeReqA));
  memcpy_P(&buf[12], timeReqB, sizeof(timeReqB));

  wifi.send((const uint8_t*)buf, 48);
  //uint32_t len = wifi.recv(upd_id, buffer, sizeof(buffer), 10000);
  len = wifi.recv(buffer, sizeof(buffer), 10000);
}
void unregisterUDP() {
  if (wifi.unregisterUDP()) {
    Serial.print("unregister udp ");
    Serial.println(" ok");
  } else {
    Serial.print("unregister udp ");
    Serial.println(" err");
  }
}

String currenttime;
//*****串口打印日期时间*****
void serialClockDisplay(int _year, int _month, int _day, int _hour, int _minute, int _second) {
  currenttime = "";
  if (_year < 1000) {
    currenttime += "20";
    Serial.print("20");
  }
  currenttime += _year;
  Serial.print(_year, DEC);
  currenttime += "-";
  Serial.print('/');
  if (_month < 10) {
    currenttime += "0";
    Serial.print("0");
  }
  currenttime += _month;
  Serial.print(_month, DEC);
  currenttime += "-";
  Serial.print('/');
  if (_day < 10) {
    currenttime += "0";
    Serial.print("0");
  }
  currenttime += _day;
  Serial.print(_day, DEC);
  currenttime += "-";
  Serial.print("   ");
  currenttime += _hour;
  Serial.print(_hour, DEC);
  currenttime += "-";
  Serial.print(':');
  if (_minute < 10) {
    currenttime += "0";
    Serial.print("0");
  }
  currenttime += _minute;
  Serial.print(_minute, DEC);
  currenttime += "-";
  Serial.print(':');
  if (_second < 10) {
    currenttime += "0";
    Serial.print("0");
  }
  currenttime += _second;
  Serial.println(_second, DEC);
  Serial.println();
}


void gettime()
{
    if (now() != prevDisplay) {
    prevDisplay = now();
    serialClockDisplay(year(), month(), day(), hour(), minute(), second());
  }
}

//Time END
/////////////////////////






//////////////////////////
//Send Data START

void wifiSendDataToServer()
{

  Serial.print("wifiSendDataToServer\r\n");
  //如果没有连着wifi 重新连一下
  if (!connectflag)
  {
    if (wifi.joinAP(SSID_NAME, PASSWORD)) {
      Serial.print("Join AP success\r\n");
      Serial.print("IP:");
      Serial.println( wifi.getLocalIP().c_str());
      connectflag = true;
    } else {
      Serial.print("Join AP failure\r\n");
      return;
    }
    if (wifi.disableMUX()) {
      Serial.print("signal ok\r\n");
    } else {
      Serial.print("signal ok\r\n");
      return;
    }
  }
  

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    Serial.print("create tcp ok\r\n");
  } else {
    Serial.print("create tcp err\r\n");
    return;
  }

  // Trying 119.23.248.167...
  // TCP_NODELAY set 
  // Connected to 119.23.248.167 (119.23.248.167) port 9527 (#0)
  // GET /all_questions HTTP/1.1
  // Host: 119.23.248.167:9527
  // User-Agent: curl/7.64.1
  // Accept: */* 
  //
  //temperature
  //heartrate
  //char senddata[] = "POST / HTTP/1.1\r\nHost: 119.23.248.167:9527\r\nAccept: */*\r\n\r\n aaa";
  //char *hello = "GET / HTTP/1.1\r\nHost: 119.23.248.167\r\nConnection: close\r\nAccept: */*\r\n\r\n";
  String senddata = "POST / HTTP/1.1\r\nHost: ";
  senddata += HOST_NAME;    //IP
  senddata += ":";
  senddata += 9527;         //port
  senddata += "\r\nAccept: */*\r\n\r\n";


  //AT+CIPSTAMAC? +CIPSTAMAC:"60:01:94:20:18:57"
  String macaddr = wifi.getStationMac();
  int startpos = macaddr.indexOf('"');
  
  senddata += "mac:";
  senddata += macaddr.substring(startpos + 1, startpos + 18);
  senddata += "\r\n";
  
  senddata += "temperature:";
  senddata += temperature;
  senddata += "\r\n";
  
  senddata += "heartrate:";
  senddata += heartrate; 
  senddata += "\r\n";

  senddata += "weight:";
  senddata += weight; 
  senddata += "\r\n";

  
  senddata += "alcohol:";
  senddata += alcohol; 
  senddata += "\r\n";

  senddata += "time:";
  senddata += currenttime; 
  senddata += "\r\n";
  
  Serial.print("Send:[\r\n");
  Serial.print(senddata.c_str());
  Serial.print("]\r\n");
  wifi.send((const uint8_t*)senddata.c_str(), senddata.length());

  /*
  uint8_t buffer[1024] = {0};
  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  if (len > 0) {
    Serial.print("Received:[\r\n");
    for (uint32_t i = 0; i < len; i++) {
      Serial.print((char)buffer[i]);
    }
    Serial.print("]\r\n");
  }
  */
  if (wifi.releaseTCP()) {
    Serial.print("release tcp ok\r\n");
  } else {
    Serial.print("release tcp err\r\n");
    return;
  }
}

//Send Data END
//////////////////////////





//WIFI END
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//LED START

#include <U8glib.h>
#include <math.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);//The definition OLED connection
float tempOLED, humiOLED;

#define INTERVAL_OLED 1000

static const unsigned char wendu[6][60] PROGMEM={

// 西安
{0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0xE6,0x03,0x80,0x19,0x00,0x80,0x19,0x00,0x80,
0x19,0x00,0xF8,0xFF,0x03,0x88,0x19,0x01,0x88,0x18,0x01,0x88,0x18,0x01,0x88,0x18,
0x01,0x88,0x18,0x01,0x48,0x18,0x01,0x28,0xF0,0x01,0x18,0x00,0x01,0x08,0x00,0x01,
0xF8,0xFF,0x01,0x08,0x00,0x01,0x08,0x00,0x01,0x00,0x00,0x00},/*"西",0*/
/* (20 X 20 , 宋体 )*/

{0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0xF0,0xFF,0x03,0x08,
0x00,0x01,0x0C,0x81,0x00,0x00,0x03,0x00,0x00,0x01,0x00,0xFC,0xDF,0x03,0x80,0x20,
0x00,0xC0,0x20,0x00,0x40,0x10,0x00,0x60,0x18,0x00,0x80,0x0B,0x00,0x00,0x34,0x00,
0x00,0xC3,0x01,0xC0,0x00,0x03,0x38,0x00,0x02,0x00,0x00,0x00},/*"安",1*/
/* (20 X 20 , 宋体 )*/

// 工业
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF8,0xF9,0x01,0x00,0x06,0x00,0x00,
0x06,0x00,0x00,0x06,0x00,0x00,0x06,0x00,0x00,0x06,0x00,0x00,0x06,0x00,0x00,0x06,
0x00,0x00,0x06,0x00,0x00,0x06,0x00,0x00,0x06,0x00,0x00,0x06,0x00,0x00,0x06,0x00,
0xFC,0xFF,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"工",0*/
/* (20 X 20 , 宋体 )*/

0x00,0x00,0x00,0x80,0x10,0x00,0x80,0x10,0x00,0x80,0x10,0x00,0x80,0x10,0x00,0x80,
0x10,0x02,0x84,0x10,0x03,0x88,0x10,0x01,0x88,0x90,0x01,0x90,0x90,0x00,0xB0,0x50,
0x00,0xB0,0x50,0x00,0xA0,0x30,0x00,0x80,0x10,0x00,0x80,0x10,0x00,0x80,0x10,0x02,
0xFC,0xFF,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"业",1*/
/* (20 X 20 , 宋体 )*/

// 大学
0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,
0x02,0x02,0xFC,0xFF,0x07,0x00,0x02,0x00,0x00,0x06,0x00,0x00,0x06,0x00,0x00,0x0A,
0x00,0x00,0x0B,0x00,0x00,0x11,0x00,0x80,0x11,0x00,0x80,0x20,0x00,0x40,0x40,0x00,
0x20,0x80,0x01,0x18,0x00,0x03,0x06,0x00,0x00,0x00,0x00,0x00,/*"大",0*/
/* (20 X 20 , 宋体 )*/

0x00,0x00,0x00,0x00,0x41,0x00,0x20,0xC2,0x00,0x60,0x46,0x00,0x60,0x26,0x00,0x40,
0x20,0x00,0xF0,0xFF,0x03,0x08,0x00,0x02,0xFC,0x7F,0x01,0x04,0x30,0x00,0x00,0x08,
0x00,0x00,0x04,0x00,0xFC,0xFF,0x03,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,
0x00,0x04,0x00,0x80,0x07,0x00,0x00,0x02,0x00,0x00,0x00,0x00,/*"学",1*/
/* (20 X 20 , 宋体 )*/

};


//LED输出
void draw(void)
{
  //================Microduino====================//
  u8g.setFont(u8g_font_fixed_v0r);
  u8g.drawStr(0, 10, "test");
  if (connectflag)
  {
    u8g.drawStr(30, 10, "WIFI");
    //u8g.drawStr(0, 55, "IP:");
    //u8g.setPrintPos(33, 45);//换行显示须再定义
    //Serial.println(wifi.getLocalIP().c_str());
    //u8g.drawStr(20, 55, wifi.getLocalIP().c_str());
  }
  else
  {
    u8g.drawStr(30, 10, "NULL");
    //u8g.drawStr(0, 55, "IP:");
    //u8g.setPrintPos(33, 45);//换行显示须再定义
    //u8g.drawStr(20, 55, "NULL");
  }
  u8g.drawLine(0, 15, 60, 15);
  u8g.drawLine(0, 0, 128, 0);
  u8g.drawLine(0, 63, 128, 63);
  u8g.drawLine(0, 63, 128, 63);
  
  u8g.drawStr(0, 25, "TEM:");    //(x, y)   //横 纵   //温度
  u8g.setPrintPos(25, 25);//换行显示须再定义
  u8g.print(temperature, DEC);
  u8g.drawStr(47, 25, "CEL");
  
  //u8g.drawLine(0, 63, 128, 63);                    
  //u8g.setPrintPos(25, 40);//换行显示须再定义
  
  u8g.drawStr(0, 35, "HEAT:");                        //心率
  u8g.setPrintPos(33, 35);//换行显示须再定义
  u8g.print(heartrate, DEC);
  u8g.drawStr(50, 35, "/MIN");

  
  //u8g.drawStr(0, 45, "SSID:");                      //wifi 名
  //u8g.setPrintPos(33, 45);//换行显示须再定义
  //u8g.drawStr(33, 45, SSID_NAME);

  
  u8g.drawStr(0, 45, "WEIGHT:");                        //体重
  u8g.setPrintPos(42, 45);//换行显示须再定义
  u8g.print(weight, DEC);
  u8g.drawStr(68, 45, " g");


  u8g.drawStr(0, 55, "ALCOHOL:");                        //体重
  u8g.setPrintPos(48, 55);//换行显示须再定义
  u8g.print(alcohol, DEC);
  //u8g.drawStr(65, 55, "/ml");
  

  
  //================ Graph===================//

//汉字
 u8g.drawXBMP( 78, 0,20, 20, wendu[0]);
 u8g.drawXBMP( 105, 0,20, 20, wendu[1]);
 u8g.drawXBMP( 78, 21,20, 20, wendu[2]);
 u8g.drawXBMP( 105, 21,20, 20, wendu[3]);
 u8g.drawXBMP( 78, 42,20, 20, wendu[4]);
 u8g.drawXBMP( 105 , 42,20, 20, wendu[5]);

}

void drawMenu()
{
  u8g.firstPage();
  do {
    draw();
  }
  while (u8g.nextPage());
}

//LED END
/////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//Metro Thread START


#include <Metro.h> 

boolean blink1State = false;   //定义 blink1State 为false状态
boolean blink2State = false;   //定义 blink2State 为false状态  

Metro blink1Metro = Metro(1000 * 5);   //把 blink1Metro 实例化 Metro 对象 ，并设置间隔时间        //5秒刷新一次屏幕
Metro blink2Metro = Metro(1000 * 30);     //把 blink2Metro 实例化 Metro 对象 ，并设置间隔时间     //30秒发送一次数据

void MetroThread()
{
   if(blink1Metro.check()){  //检查blink1Metro间隔(我的理解是计时器每隔100毫秒就会返回true，就执行以下程序)
    blink1State = !blink1State;   //反转blink1State的逻辑真或假（false or true）
    Serial.print("blink1Metro\r\n");
    //LED屏 显示
    drawMenu();
   }
 
  if(blink2Metro.check()){
    blink2State = !blink2State;
    Serial.print("blink2Metro\r\n");
    //wifi
    wifiSendDataToServer();
  }  
}

//Metro Thread START
////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200); //
  // serial
  pinMode(A0, INPUT);   //心率模块

  termo.begin();    //温度模块 
  //delay(100);

  pinMode(gas_din,INPUT);       //酒精模块
  pinMode(gas_ain,INPUT);       //酒精模块

  
  //wifi
  wifiInit();


  //先显示一遍界面
  //LED屏 显示
  drawMenu();

  updateTimeData();   //更新时间
}

// the setup routine runs once when you press reset:
///////////////////////////////////////////////////////////////


void loop() {
  // print out the value you read:
  //Serial.print("t0:");
  //Serial.print(digitalRead(A0) * 2000);
  //Serial.println();
  //delay(1000);        // delay in between reads for stability
  
  //获取传感器数据  温度
  getSensorData();
  
  MetroThread();
  
}
