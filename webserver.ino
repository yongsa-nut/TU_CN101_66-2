#include <WiFi.h>
#include <WebSocketsServer.h>
#include <esp32cam.h>

const char* ssid = "OnePlus 11 5G"; //Change to your wifi name
const char* password = "u38ujyay";  //Change to your wifi password

WebSocketsServer webSocket = WebSocketsServer(81);
static auto res = esp32cam::Resolution::find(600, 480);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.printf("WebSocket client #%u connected\n", num);
      webSocket.sendTXT(num, "Connected to ESP32 CAM");
      break;
    case WStype_TEXT:
      if (strcmp((char*)payload, "capture") == 0) {
        auto frame = esp32cam::capture();
        if (frame == nullptr) {
          Serial.println("Failed to capture image");
        }else{
          webSocket.sendBIN(num, frame->data(), frame->size());
        }
      }
      Serial.println((char*)payload);
      break;
    case WStype_DISCONNECTED:
      Serial.printf("WebSocket client #%u disconnected\n", num);
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize the camera
  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(res);
    cfg.setBufferCount(2);
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "Camera OK" : "Camera Failed");
  }

  // Set up the WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
}
