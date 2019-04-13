//Copyright (c) 2019 aNoŒ¤ ƒvƒŠƒ“‚ğç‚é‹Zp 
//https://github.com/anoken/purin_wo_mamoru_gijutsu/
//

#include "AquesTalkTTS.h"
#include <M5Stack.h>
#define AQUESTALK_KEY "xxxx"
void setup() {
  M5.begin();
  int iret = TTS.create(AQUESTALK_KEY);
  TTS.play("purin,ga,torareta", 80);
}
void loop() {
  M5.update();
}
