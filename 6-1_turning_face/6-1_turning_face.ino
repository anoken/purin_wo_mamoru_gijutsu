#include <M5Stack.h>
#include "utility/MPU9250.h"
TFT_eSprite *Spr;
uint32_t front_col = TFT_BLACK;
uint32_t back_col = TFT_YELLOW;
MPU9250 IMU;
float rot_theta=0.0;
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
void setup() {
  M5.begin();
  Wire.begin();		//I2Cの初期化 
  /* 加速度センサーの初期化 */
  IMU.calibrateMPU9250(IMU.gyroBias, IMU.accelBias);
  IMU.initMPU9250();
  Spr = new TFT_eSprite(&M5.Lcd);	//Sprite処理の初期化
  /* お顔表示のスレッドを生成する */
  xTaskCreatePinnedToCore(drawLoop,"drawLoop",4096,NULL,1,NULL,0);
}
void loop() {
  M5.update();
   /*加速度センサーの読み取り*/
  if (IMU.readByte(MPU9250_ADDRESS,INT_STATUS) & 0x01)  {
   IMU.readAccelData(IMU.accelCount);
   IMU.getAres();
   IMU.ax = (float)IMU.accelCount[0] * IMU.aRes ;
   IMU.ay = (float)IMU.accelCount[1] * IMU.aRes + 1;
   IMU.az = (float)IMU.accelCount[2] * IMU.aRes ;
   /*加速度センサーから傾きを求め、移動平均で平滑化する*/
    rot_theta = 0.9 * rot_theta + 0.1 * (atan2(IMU.ax,IMU.ay) );
  }
  delay(50);
}
