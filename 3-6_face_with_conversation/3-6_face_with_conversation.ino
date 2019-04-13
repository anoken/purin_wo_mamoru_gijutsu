//Copyright (c) 2019 aNo研 プリンを守る技術 
//https://github.com/anoken/purin_wo_mamoru_gijutsu/
//

#include <M5Stack.h>
#include "AquesTalkTTS.h"
TFT_eSprite *Spr;
uint32_t front_col = TFT_BLACK;
uint32_t back_col = TFT_YELLOW;

void draw_face(float open);	//顔の表示を行う関数

void drawLoop(void *args) {
  Spr->setColorDepth(8);
  Spr->createSprite(320, 240);
  Spr->setBitmapColor(front_col, back_col);
  for (;;)  {
    int level = TTS.getLevel();
    float open = level / 250.0;
    draw_face(open);
    delay(50);
  }
}
void setup() {
  M5.begin();
  Spr = new TFT_eSprite(&M5.Lcd);
  int iret = TTS.create(NULL);
  if (iret)    Serial.println("ERR: TTS_create():");
  xTaskCreatePinnedToCore(drawLoop,"drawLoop",4096,NULL,1,NULL,0);
}
void loop() {
  M5.update();
  if (M5.BtnA.wasPressed())  TTS.play("purin,kaeshitene", 80);
  delay(50);
}
