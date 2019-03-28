#include <esp_camera.h>
#include "driver/uart.h"

//This Port is M5Camera of JP-Model
//https://www.switch-science.com/catalog/5207/
#define CAMERA_MODEL_M5STACK_PSRAM
#if defined(CAMERA_MODEL_M5STACK_PSRAM)
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     22
#define SIOC_GPIO_NUM     23
#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM        5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       32
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21
#endif

static const char* TAG = "camera";
static const int RX_BUF_SIZE = 1024;

static void uart_init();
char tx_buffer[200] = { '\0' };

void setup() {
  Serial.begin(115200);
  Serial.println();
  uart_init();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_HQVGA;
  config.jpeg_quality = 6;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  sensor_t * s = esp_camera_sensor_get();

  vTaskDelay(100 / portTICK_PERIOD_MS);
}

void loop() {
  uint32_t data_len = 0;

  tx_buffer[0] = 0xFF;
  tx_buffer[1] = 0xD8;
  tx_buffer[2] = 0xEA;
  tx_buffer[3] = 0x01;

  camera_fb_t * fb = esp_camera_fb_get();
  tx_buffer[4] = (uint8_t)((fb->len & 0xFF0000) >> 16) ;
  tx_buffer[5] = (uint8_t)((fb->len & 0x00FF00) >> 8 ) ;
  tx_buffer[6] = (uint8_t)((fb->len & 0x0000FF) >> 0 );

  uart_write_bytes(UART_NUM_1, (char *)tx_buffer, 7);
  uart_write_bytes(UART_NUM_1, (char *)fb->buf, fb->len);
  data_len = (uint32_t)(tx_buffer[4] << 16) | (tx_buffer[5] << 8) | tx_buffer[6];
  printf("should %d, print a image, len: %d\r\n", fb->len, data_len);
  esp_camera_fb_return(fb);


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
  uart_set_pin(UART_NUM_1, 13, 4, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
}
