//Copyright (c) 2019 aNo研 プリンを守る技術 
//https://github.com/anoken/purin_wo_mamoru_gijutsu/
//

#include <M5Stack.h>
#include "AquesTalkTTS.h"
const double V_REF=3.00;

/* GROVE-I2C ADCを初期化する */
void init_adc() {
   Wire.begin();
   Wire.beginTransmission(0x50);
   Wire.write(0x02);	
   Wire.write(0x20);
   Wire.endTransmission();
}
/* GROVE-I2C ADCから圧力センサーのアナログ値を読み出す */
float read_adc() {
   unsigned int getData;
   static float analogVal = 0;   
   Wire.beginTransmission(0x50); 
   Wire.write(0x00);             
   Wire.endTransmission();
   Wire.requestFrom(0x50, 2);    
   delay(1);
  if (Wire.available() <= 2)  {
    getData = (Wire.read() & 0x0f) << 8;
    getData |= Wire.read();
  }
   delay(5);
   analogVal = 0.9*analogVal+0.1*getData*V_REF*2.0/4096.0;
   return analogVal;
}
void setup() {
   M5.begin();
   init_adc();
}
void loop() {
  const double thresh_max = 3.5;	//プリンの閾値_最大
  const double thresh_min = 3.0;	//プリンの閾値_最小
  const double thresh_none = 1.0;	//なにも置いていないときの重さ
  float value = read_adc();

  /*プリンの重さが許容範囲内であればなにもしない*/
  if ((value < thresh_max)||(value >= thresh_min)) {  
	 delay(10);
  }
  /* プリンの重さが違う時は「プリンじゃないよ」としゃべる */
  else if (value > thresh_none) {  
	 TTS.play("purin,ja,naiyo", 100);
	 delay(1000);
  }
  /* 何も置いてない時は「プリンがとられた」としゃべる */
  else {  
	 TTS.play("purin,ga,torareta", 100);
	 delay(1000);
  }

  delay(50);
}