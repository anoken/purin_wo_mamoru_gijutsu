//Copyright (c) 2019 aNo研 プリンを守る技術 
//https://github.com/anoken/purin_wo_mamoru_gijutsu/
//

#include <M5Stack.h>
#define SERVO_ADDR 0x53

uint8_t servo_l = 10;
uint8_t servo_r = 11;
void Servo_write_us(uint8_t number, uint16_t us) {
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x00 | number);
  Wire.write(us & 0x00ff);
  Wire.write(us >> 8 & 0x00ff);
  Wire.endTransmission();
}
void setup() {
  M5.begin(true, false, true);
  Wire.begin(21, 22, 100000);
  Servo_write_us(servo_l, 1500); //停止する
  Servo_write_us(servo_r, 1500); //停止する
}
void loop() {
  M5.update();
  if (M5.BtnA.wasPressed()) {//CW方向に回転する
    Servo_write_us(servo_l, 1000);  //左タイヤがCW回転する
    Servo_write_us(servo_r, 1000); //右タイヤがCCW回転する
  }
  if (M5.BtnB.wasPressed()) {
    Servo_write_us(servo_l, 1500); //停止する
    Servo_write_us(servo_r, 1500); //停止する
  }
  if (M5.BtnC.wasPressed()) {//CCW方向に回転する
    Servo_write_us(servo_l, 2000); //左タイヤがCW回転する
    Servo_write_us(servo_r, 2000);  //右タイヤがCCW回転する
  }
  delay(100);
}
