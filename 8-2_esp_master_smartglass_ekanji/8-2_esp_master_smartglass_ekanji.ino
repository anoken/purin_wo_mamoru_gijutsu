//Copyright (c) 2019 aNo研 プリンを守る技術 
//https://github.com/anoken/purin_wo_mamoru_gijutsu/
//

/*  ESP-Nowの Master側の通信フロー
  Step1：WifiをSTAモードで起動する
  Step2：Master上のESP-Now を初期化する
  Step3：SlaveESP32を探す
  Step4：SlaveのESP32が発見されたら、peerとしてSlaveを追加する
  Step5：MasterからSlaveへのデータ送信を開始する
*/

#include <esp_now.h>
#include <WiFi.h>
#include <M5Stack.h>


#include "esp_now_inc.h" //https://github.com/anoken/purin_wo_mamoru_gijutsu
void InitESPNow();  //Step2_Master上のESP-Now を初期化する
void ScanForSlave();  //Step3_SlaveESP32を探す
bool manageSlave(); //Step4_SlaveのESP32を追加する
void deletePeer();  //ESP-NOWのpeerを削除する


uint8_t servo_l = 10;
uint8_t servo_r = 11;
void sendData() { //Step5_MasterからSlaveへのデータ送信を開始する
  uint8_t data[2];
  data[0] = servo_l;  data[1] = servo_r;
  const uint8_t *peer_addr = slave.peer_addr;
  esp_err_t result = esp_now_send(peer_addr, data, 2);
  Serial.print("send status: ");
  if (result == ESP_OK)   Serial.println("success");
  else    Serial.println("error");
}
void setup() {
  M5.begin();
  WiFi.mode(WIFI_STA);  //Step1
  InitESPNow();   //Step2
}
void loop() {
  M5.update();
  /* M5のボタンで動作を切り替える */
  if (M5.BtnA.isPressed()) {
    servo_l = 1000; servo_r = 1000;
  }
  else if (M5.BtnB.isPressed()) {
    servo_l = 1000; servo_r = 2000;
  }
  else if (M5.BtnC.isPressed()) {
    servo_l = 2000; servo_r = 2000;
  }
  else {
    servo_l = 1500; servo_r = 1500;
  }
  if (slave.channel == CHANNEL) {
    bool isPaired = manageSlave();  //Step4
    if (isPaired) {
      sendData();     //Step5
    } else {
      Serial.println("Slave pair failed!");
    }
  }
  else {
    ScanForSlave();     //Step3
  }
  delay(20);
}
