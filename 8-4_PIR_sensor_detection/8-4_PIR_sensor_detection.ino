//Copyright (c) 2019 aNo研 プリンを守る技術 
//https://github.com/anoken/purin_wo_mamoru_gijutsu/
//

#include <M5Stack.h>
#include "AquesTalkTTS.h"
const int PIN_PIR_SIG = 22;	//GROVE-A Port

void setup() {
  M5.begin();
  pinMode( PIN_PIR_SIG, INPUT );
  int iret = TTS.createK("");
}
void loop() {
  M5.update();
  bool snr = digitalRead( PIN_PIR_SIG );
  static bool snr_old = snr;
  /* 人を検出したタイミングで、「あちらのお客様からです」と発声する。 */
  if ((snr == HIGH) && (snr_old == LOW)) {
  TTS.playK("achirano,okyakusama,karadesu", 100);
  }

  snr_old = snr;
}
