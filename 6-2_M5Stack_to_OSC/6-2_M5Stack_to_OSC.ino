#include <M5Stack.h>
#include <ArduinoOSC.h>
#include "AquesTalkTTS.h"

WiFiUDP udp;
ArduinoOSCWiFi osc;
const int recv_port = 10000;
#define AQUESTALK_KEY ""

/* OSCでメッセージを受信する */
void messageCb(OSCMessage& m) {
  String str = m.getArgAsString(0);
  int len = str.length();
  String cmd = str.substring(0, 4);	//最初の4文字を切り取る
  String value = str.substring(5);	//5文字目以降を切り取る
  if (cmd == "talk") {			//最初の4文字がtalkなら
    TTS.playK(value.c_str(), 100);	//5文字目以降をしゃべる
    　/*AquesTalkTTSで日本語対応のplayKを使用*/

    delay(3000);
  }
}
void setup() {
  M5.begin();
  wifi_connect();
  /* OSCの設定をする */
  osc.begin(udp, recv_port);
  osc.addCallback("/msg", &messageCb);
/*AquesTalkTTSで日本語対応のcreateKを使用*/
  int iret = TTS.createK(AQUESTALK_KEY);
}
void loop() {
  osc.parse();
}

 /* Wifiに接続する */
void wifi_connect() {  
  const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_pass";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)  delay(500);
  Serial.println(WiFi.localIP());
}
