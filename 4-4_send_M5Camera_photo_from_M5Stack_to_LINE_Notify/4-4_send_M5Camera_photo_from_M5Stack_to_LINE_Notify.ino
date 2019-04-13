//Copyright (c) 2019 aNo研 プリンを守る技術 
//https://github.com/anoken/purin_wo_mamoru_gijutsu/
//

#include <M5Stack.h>
#include <ssl_client.h>
#include <WiFiClientSecure.h>
#include "M5Stack.h"
#include "driver/uart.h"

#define SSID        "yourwifi"
#define PASSWORD     "yourpass"
const char* host = "notify-api.line.me";
const char* token = "your_token";

/* Define ------------------------------------------------------------ */
typedef enum {
  WAIT_FRAME_1 = 0x00,
  WAIT_FRAME_2,
  WAIT_FRAME_3,
  GET_NUM,
  GET_LENGTH,
  GET_MSG,
  RECV_FINISH,
  RECV_ERROR,
} uart_rev_state_t;

typedef struct {
  uint8_t idle;
  uint32_t length;
  uint8_t *buf;
} jpeg_data_t;


/* Global Var ----------------------------------------------------------- */
uart_rev_state_t uart_rev_state;
static const int RX_BUF_SIZE = 1024 * 40;
static const uint8_t frame_data_begin[3] = { 0xFF, 0xD8, 0xEA };

jpeg_data_t jpeg_data_1;
jpeg_data_t jpeg_data_2;

/* Static fun ------------------------------------------------------------ */
static void uart_init();
static void uart_msg_task(void *pvParameters);

//Line通知
void sendLineNotify(uint8_t* image_data, size_t image_sz) {
  WiFiClientSecure client;
  if (!client.connect(host, 443))   return;
  int httpCode = 404;
  size_t image_size = image_sz;
  String boundary = "----purin_alert--";
  String body = "--" + boundary + "\r\n";
  String message = "プリンが取られました！！！";
  body += "Content-Disposition: form-data; name=\"message\"\r\n\r\n" + message + " \r\n";
  if (image_data != NULL && image_sz > 0 ) {
    image_size = image_sz;
    body += "--" + boundary + "\r\n";
    body += "Content-Disposition: form-data; name=\"imageFile\"; filename=\"image.jpg\"\r\n";
    body += "Content-Type: image/jpeg\r\n\r\n";
  }
  String body_end = "--" + boundary + "--\r\n";
  size_t body_length = body.length() + image_size + body_end.length();
  String header = "POST /api/notify HTTP/1.1\r\n";
  header += "Host: notify-api.line.me\r\n";
  header += "Authorization: Bearer " + String(token) + "\r\n";
  header += "User-Agent: " + String("M5Stack") + "\r\n";
  header += "Connection: close\r\n";
  header += "Cache-Control: no-cache\r\n";
  header += "Content-Length: " + String(body_length) + "\r\n";
  header += "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n\r\n";
  client.print( header + body);
  Serial.print(header + body);

  bool Success_h = false;
  uint8_t line_try = 3;
  while (!Success_h && line_try-- > 0) {
    if (image_size > 0) {
      size_t BUF_SIZE = 1024;
      if ( image_data != NULL) {
        uint8_t *p = image_data;
        size_t sz = image_size;
        while ( p != NULL && sz) {
          if ( sz >= BUF_SIZE) {
            client.write( p, BUF_SIZE);
            p += BUF_SIZE; sz -= BUF_SIZE;
          } else {
            client.write( p, sz);
            p += sz; sz = 0;
          }
        }
      }



      client.print("\r\n" + body_end);
      Serial.print("\r\n" + body_end);

      while ( client.connected() && !client.available()) delay(10);
      if ( client.connected() && client.available() ) {
        String resp = client.readStringUntil('\n');
        httpCode    = resp.substring(resp.indexOf(" ") + 1, resp.indexOf(" ", resp.indexOf(" ") + 1)).toInt();
        Success_h   = (httpCode == 200);
        Serial.println(resp);
      }
      delay(10);
    }
  }
  client.stop();
}

void setup() {

  delay(500);
  M5.begin(true, false, true);
  uart_init();
  jpeg_data_1.buf = (uint8_t *) malloc(sizeof(uint8_t) * 1024 * 37);
  if (jpeg_data_1.buf == NULL) {
    Serial.println("malloc jpeg buffer 1 error");
  }
  jpeg_data_2.buf = (uint8_t *) malloc(sizeof(uint8_t) * 1024 * 37);
  if (jpeg_data_2.buf == NULL) {
    Serial.println("malloc jpeg buffer 2 error");
  }
  xTaskCreatePinnedToCore(uart_msg_task, "uart_task", 3 * 1024, NULL, 1, NULL, 0);
  Serial.print("Connecting to " + String(SSID));
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  M5.update();
  delay(50);
  uint32_t data_len = 0;
  uint8_t rx_buffer[21] = { '\0' };
  if (M5.BtnA.wasPressed())  {
    if (jpeg_data_1.idle == 1) {
      jpeg_data_1.idle = 2;
      M5.lcd.drawJpg(jpeg_data_1.buf, jpeg_data_1.length, 40, 30);
      sendLineNotify(jpeg_data_1.buf, jpeg_data_1.length);
      jpeg_data_1.idle = 0;
    }
    else if (jpeg_data_2.idle == 1) {
      jpeg_data_2.idle = 2;
      M5.lcd.drawJpg(jpeg_data_2.buf, jpeg_data_2.length, 40, 30);
      sendLineNotify(jpeg_data_2.buf, jpeg_data_2.length);
      jpeg_data_2.idle = 0;
    }
    delay(2000);
  }
}


static void uart_init() {
  const uart_config_t uart_config = {
    .baud_rate = 921600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };
  uart_param_config(UART_NUM_1, &uart_config);
  uart_set_pin(UART_NUM_1, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_driver_install(UART_NUM_1, RX_BUF_SIZE, 0, 0, NULL, 0);
}

static void uart_msg_task(void *pvParameters) {
  uint32_t data_len = 0;
  uint32_t length = 0;
  uint8_t rx_buffer[10];
  uint8_t buffer_use = 0;
  while (true) {
    // delay(1);
    uart_get_buffered_data_len(UART_NUM_1, &data_len);
    if (data_len > 0) {
      switch (uart_rev_state) {
        case WAIT_FRAME_1:
          uart_read_bytes(UART_NUM_1, (uint8_t *)&rx_buffer, 1, 10);
          if (rx_buffer[0] == frame_data_begin[0]) {
            uart_rev_state = WAIT_FRAME_2;
          }
          else {
            break ;
          }

        case WAIT_FRAME_2:
          uart_read_bytes(UART_NUM_1, (uint8_t *)&rx_buffer, 1, 10);
          if (rx_buffer[0] == frame_data_begin[1]) {
            uart_rev_state = WAIT_FRAME_3;
          } else {
            uart_rev_state = WAIT_FRAME_1;
            break ;
          }

        case WAIT_FRAME_3:
          uart_read_bytes(UART_NUM_1, (uint8_t *)&rx_buffer, 1, 10);
          if (rx_buffer[0] == frame_data_begin[2]) {
            uart_rev_state = GET_NUM;
          } else {
            uart_rev_state = WAIT_FRAME_1;
            break ;
          }

        case GET_NUM:
          uart_read_bytes(UART_NUM_1, (uint8_t *)&rx_buffer, 1, 10);
          Serial.printf("get number cam buf %d\t", rx_buffer[0]);
          uart_rev_state = GET_LENGTH;

        case GET_LENGTH:
          uart_read_bytes(UART_NUM_1, (uint8_t *)&rx_buffer, 3, 10);
          data_len = (uint32_t)(rx_buffer[0] << 16) | (rx_buffer[1] << 8) | rx_buffer[2];
          Serial.printf("data length %d\r\n", data_len);

        case GET_MSG:
          if (buffer_use == 0) {
            buffer_use = 1;
            if (jpeg_data_1.idle == 0) {
              if (uart_read_bytes(UART_NUM_1, jpeg_data_1.buf, data_len, 10) == -1) {
                uart_rev_state = RECV_ERROR;
                break ;
              }
              jpeg_data_1.length = data_len;
              jpeg_data_1.idle = 1;
            }
          } else {
            buffer_use = 0;
            if (jpeg_data_2.idle == 0) {
              if (uart_read_bytes(UART_NUM_1, jpeg_data_2.buf, data_len, 10) == -1) {
                uart_rev_state = RECV_ERROR;
                break ;
              }
              jpeg_data_2.length = data_len;
              jpeg_data_2.idle = 1;
            }
          }
          Serial.printf("get image %d buffer", buffer_use);
          uart_rev_state = RECV_FINISH;

        case RECV_FINISH:
          Serial.printf("get image finish...\r\n");
          uart_rev_state = WAIT_FRAME_1;
          break ;

        case RECV_ERROR:
          Serial.printf("get image error\r\n");
          uart_rev_state = WAIT_FRAME_1;
          break ;
      }
    } else {
      vTaskDelay(10 / portTICK_RATE_MS);
    }

  }
  vTaskDelete(NULL);
}