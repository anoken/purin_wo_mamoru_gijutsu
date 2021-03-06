//Copyright (c) 2019 aNo研 プリンを守る技術 
//https://github.com/anoken/purin_wo_mamoru_gijutsu/

#include <ssl_client.h>
#include <WiFiClientSecure.h>
#include <M5Stack.h>

/* LINENofifyの設定 */
const char* host = "notify-api.line.me";
const char* token = "your_token";

void setup() {
  M5.begin();		//M5Stackを起動する
  wifi_connect();		//Wifiに接続する
  send_line_alert();	//LINEへの送信
}

void loop() {
  M5.update();		//M5Stackの内部処理を更新
  delay(500);
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
  const char* message1 = "プリンが取られました！！！";
  WiFiClientSecure client;
  if (!client.connect(host, 443)) {
    return;
  }
  /* LINE NotifyのサーバへHTTPS通信でメッセージをPOSTする */
  String query = "message=" + String(message1);
  
  String request = String("") +
                   "POST /api/notify HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Authorization: Bearer " + token + "\r\n" +
                   "Content-Length: " + String(query.length()) +  "\r\n" +
                   "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
                   query + "\r\n";

  client.print(request);
  while (client.connected()) {
    String line_str = client.readStringUntil('\n');
    if (line_str == "\r")   break;
  }
}
