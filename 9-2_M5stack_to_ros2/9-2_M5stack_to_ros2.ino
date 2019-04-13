//Copyright (c) 2019 aNo研 プリンを守る技術 
//https://github.com/anoken/purin_wo_mamoru_gijutsu/
//


#include <ros2arduino.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#define AGENT_IP   "0.0.0.0"  //AGENT IP address
#define AGENT_PORT 2018   //AGENT port number
#define SERVO_ADDR 0x53
WiFiUDP udp;

void wifi_connect();	// Wifiに接続する 		
void Servo_write_us(uint8_t number, uint16_t us);//サーボモータを制御する

uint8_t servo_l = 10;
uint8_t servo_r = 11;

void subscribePorter(geometry_msgs::Vector3  * msg, void* arg) {
  (void)(arg);
  int speed_l=msg->x;
  int speed_r=msg->y;

  Servo_write_us(servo_l, speed_l); //停止する
  Servo_write_us(servo_r, speed_r); //停止する
}

class PorterSub : public ros2::Node {
  public: PorterSub(): Node() {
      this->createSubscriber<geometry_msgs ::Vector3>("motor",
          (ros2::CallbackFunc)subscribePorter, NULL);
    }
};

void setup() {
  M5.begin(true, false, true);
  Wire.begin(21, 22, 100000);
  wifi_connect();
  ros2::init(&udp, AGENT_IP, AGENT_PORT);
}

void loop() {
  M5.update();
  delay(50);
  static PorterSub PorterNode;
  ros2::spin(&PorterNode);
}

void Servo_write_us(uint8_t number, uint16_t us) {
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x00 | number);
  Wire.write(us & 0x00ff);
  Wire.write(us >> 8 & 0x00ff);
  Wire.endTransmission();
}
 /* Wifiに接続する */
void wifi_connect() {	
  const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_pass";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)  delay(500);
  Serial.println(WiFi.localIP());
}