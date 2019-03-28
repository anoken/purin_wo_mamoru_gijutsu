/*  ESP-Nowの Slave側の通信フロー
  Step1：WifiをWIFI_APモードで起動する
  Step2：WIFI_APモードで、SlaveのSSIDとパスワードを設定する
  Step3：SlaveのESP-Now を初期化
  Step4：受信コールバックに登録してデータを待つ
  Step5：Masterからのデータを受信する
*/
#include <M5Stack.h>
#include <esp_now.h>
#include <WiFi.h>
#include "esp_now_inc.h" //https://github.com/anoken/purin_wo_mamoru_gijutsu
void InitESPNow();
void Servo_write_us(uint8_t number, uint16_t us);
uint8_t servo_l = 10;
uint8_t servo_r = 11;
#define SERVO_ADDR 0x53
void Servo_write_us(uint8_t number, uint16_t us) {
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x00 | number);
  Wire.write(us & 0x00ff);
  Wire.write(us >> 8 & 0x00ff);
  Wire.endTransmission();
}
void setup() {
  M5.begin();
  WiFi.mode(WIFI_AP);       //Step1
  configDeviceAP();         //Step2
  Serial.print("AP MAC:"); Serial.println(WiFi.softAPmacAddress());
  InitESPNow();         //Step3
  esp_now_register_recv_cb(OnDataRecv);   //Step4
}

//Step5
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  Servo_write_us(servo_l, data[0]); //停止する
  Servo_write_us(servo_r, data[1]); //停止する
  delay(1);
}

void loop() {
  M5.update();
  delay(10);
}

void configDeviceAP() {     //Step2
  char* SSID = "your_ssid";
  bool result = WiFi.softAP(SSID, "your_password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("Success_AP: " + String(SSID));
  }
}
