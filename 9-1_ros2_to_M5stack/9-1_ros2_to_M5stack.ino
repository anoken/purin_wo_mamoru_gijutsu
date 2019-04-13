//Copyright (c) 2019 aNo研 プリンを守る技術 
//https://github.com/anoken/purin_wo_mamoru_gijutsu/
//

#include <M5Stack.h>
#include "utility/MPU9250.h"
#include <ros2arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#define AGENT_IP   "0.0.0.0"  //AGENT IP address
#define AGENT_PORT 2018   //AGENT port number
#define PUBLISH_FREQUENCY 40 //hz
WiFiUDP udp;

TFT_eSprite *Spr;
uint32_t front_col = TFT_BLACK;
uint32_t back_col = TFT_YELLOW;
MPU9250 IMU;
float rot_theta;

void rot(int16_t x_in, int16_t y_in, int16_t &x_rot, int16_t &y_rot, 
float theta);
void fillTriangle_r(TFT_eSPI *spi, int16_t x0, int16_t y0, int16_t x1, 
int16_t y1, int16_t x2, int16_t y2, uint16_t color, float theta);
void fillCircle_r(TFT_eSPI *spi, int16_t x0, int16_t y0, int16_t r0,
 uint16_t color, float theta);
void drawLoop(void *args);
void wifi_connect();

void publishString_x(std_msgs::Int16* msg, void* arg) {
  (void)(arg);
  msg->data = IMU.ax *1000;
}

class StringPub : public ros2::Node {
  public: StringPub(): Node() {
      ros2::Publisher<std_msgs::Int16>* publisher_x = 
  this->createPublisher<std_msgs::Int16>("acc");
      this->createWallFreq(PUBLISH_FREQUENCY,
   (ros2::CallbackFunc)publishString_x, NULL, publisher_x);
    }
};

void setup() {
  M5.begin();
  Wire.begin();
  wifi_connect();
  ros2::init(&udp, AGENT_IP, AGENT_PORT);

  IMU.calibrateMPU9250(IMU.gyroBias, IMU.accelBias);
  IMU.initMPU9250();
  Spr = new TFT_eSprite(&M5.Lcd);
  xTaskCreatePinnedToCore(drawLoop, "drawLoop", 4096, NULL, 1, NULL, 0);
}
void loop() {
  static StringPub StringNode;
  ros2::spin(&StringNode);

  M5.update();
  if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)  {
    IMU.readAccelData(IMU.accelCount);
    IMU.getAres();
    IMU.ax = (float)IMU.accelCount[0] * IMU.aRes ;
    IMU.ay = (float)IMU.accelCount[1] * IMU.aRes + 1;
    IMU.az = (float)IMU.accelCount[2] * IMU.aRes ;
    rot_theta = 0.75 * rot_theta + 0.25 * (atan2(IMU.ax, IMU.ay) );


  }
}
 /* Wifiに接続する */
void wifi_connect() {  
  const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_pass";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)  delay(500);
  Serial.println(WiFi.localIP());
}

/* 回転させる関数 */
void rot(int16_t x_in,int16_t y_in,int16_t &x_rot,int16_t &y_rot,
  float theta) {
  x_rot = (x_in-160)*cos(theta) - (y_in-120)*sin(theta)+160;
  y_rot = (x_in-160)*sin(theta) +  (y_in-120)*cos(theta)+120;
}
/* 三角形を回転する */
void fillTriangle_r(TFT_eSPI *spi,int16_t x0,int16_t y0,int16_t x1,
  int16_t y1,int16_t x2,int16_t y2,uint16_t color,float theta) {
  int16_t x0_rot,x1_rot,x2_rot,y0_rot,y1_rot,y2_rot;
  rot(x0,y0,x0_rot,y0_rot,theta);
  rot(x1,y1,x1_rot,y1_rot,theta);
  rot(x2,y2,x2_rot,y2_rot,theta);
  spi->fillTriangle(x0_rot,y0_rot,x1_rot,y1_rot,x2_rot,y2_rot,color);
}

/* 円形を回転する */
void fillCircle_r(TFT_eSPI *spi,int16_t x0,int16_t y0,int16_t r0,
    uint16_t color,float theta) {
  int16_t x0_rot,y0_rot;
  rot(x0,y0,x0_rot,y0_rot,theta);
  spi->fillCircle( x0_rot,y0_rot, r0,  color);
}
/* お顔を表示する */
void draw_face(float open,float theta){    
    fillTriangle_r(Spr,190,70,190,50,
                   280 + random(10),25 + random(15),front_col,theta); 
    fillTriangle_r(Spr,130,70,130,50,
                   50 + random(10),25 + random(15),front_col,theta); 
    fillCircle_r(Spr,90 + random(5),93 + random(5),
                 25 ,front_col ,theta); //左目
    fillCircle_r(Spr,230 + random(5),93 + random(5),
                 25 ,front_col ,theta); //右目
    fillTriangle_r(Spr,133,188 - open / 2,133 + 60,188 - open / 2,
                   133 + 60,188 + open / 2,front_col,theta); 
    fillTriangle_r(Spr,133,188 - open / 2,133,188 + open / 2,
                   133 + 60,188 + open / 2,front_col,theta); 
}
 /* お顔表示のスレッドのメイン処理 */
void drawLoop(void *args) {
  Spr->setColorDepth(8);
  Spr->createSprite(320,240);
  Spr->setBitmapColor(front_col,back_col);
  float theta = 0;
int level = 1;
  for (;;)  {
    Spr->fillSprite(back_col); 
    float open = level / 250.0;
    theta = rot_theta;
    draw_face( open, theta);
    Spr->pushSprite(0,0);  
    delay(50);
  }
}
