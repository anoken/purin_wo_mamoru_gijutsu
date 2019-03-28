#include <ssl_client.h>
#include <WiFiClientSecure.h>
#include <M5Stack.h>

void send_line_alert();	//LINEへの送信関数
void wifi_connect() ;	//wifiに接続する関数

void setup() {
  M5.begin();
  wifi_connect();
  pinMode(36, INPUT_PULLUP);//G36をプルアップする
}

void loop() {
  M5.update();
  int buttonState = digitalRead(36);
  if (buttonState == HIGH) {	//プリンがない時にHIGH
    send_line_alert();	//LINEへメッセージを送る。
    delay(1000);
  }
  delay(10);
}

/* Wifiに接続する */
void wifi_connect() {
  const char* ssid = "your_wifi_ssid";
  const char* password = "your_wifi_pass";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)  delay(500);
  Serial.println(WiFi.localIP());
}

/* LINEへの送信 */
void send_line_alert() {
  const char* host = "notify-api.line.me";
  const char* token = "your_token";
  
  const char* message1 = "プリンが取られました！！！";
  WiFiClientSecure client;
  if (!client.connect(host, 443)) {
    return;
  }
  /* LINE NotifyのサーバへHTTPS通信でメッセージをPOSTする */
  String query = "message=" + String(message1);
  String request = String("") + "POST /api/notify HTTP/1.1\r\nHost:"
                   + host + "\r\nAuthorization:Bearer" + token + "\r\nContent-Length:"
                   + String(query.length()) +
                   "\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n"
                   + query + "\r\n";
  client.print(request);
  while (client.connected()) {
    String line_str = client.readStringUntil('\n');
    if (line_str == "\r")   break;
  }
}
