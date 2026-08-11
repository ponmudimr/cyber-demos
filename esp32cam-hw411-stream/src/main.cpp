#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// ==========================================
// 1. Wi-Fi Configuration
// ==========================================
const char* ssid     = "SPIDEY";
const char* password = "ponmudi123";

const char* ap_ssid     = "ESP32-CAM-Webcam";
const char* ap_password = "12345678Password";

// ==========================================
// 2. Hardware Pin Definitions (AI-Thinker / HW-411 Bridge Setup)
// ==========================================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define FLASH_LED_PIN      4

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

int flash_brightness = 0;

static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32-CAM Wi-Fi Live Stream</title>
  <style>
    :root {
      --bg-dark: #090d16;
      --card-bg: #131c2e;
      --card-border: #1e2d4a;
      --accent: #38bdf8;
      --accent-hover: #0284c7;
      --text-main: #f8fafc;
      --text-sub: #94a3b8;
      --danger: #ef4444;
      --success: #22c55e;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; }
    body { background: var(--bg-dark); color: var(--text-main); padding: 24px 16px; display: flex; flex-direction: column; align-items: center; min-height: 100vh; }
    
    header { text-align: center; margin-bottom: 24px; }
    h1 { color: var(--accent); font-size: 1.8rem; font-weight: 700; letter-spacing: -0.5px; margin-bottom: 6px; }
    p.subtitle { color: var(--text-sub); font-size: 0.9rem; }
    
    .badge { display: inline-flex; align-items: center; gap: 6px; padding: 4px 12px; border-radius: 20px; font-size: 0.8rem; font-weight: 600; background: rgba(34, 197, 94, 0.15); color: var(--success); border: 1px solid rgba(34, 197, 94, 0.3); margin-top: 10px; }
    .badge-dot { width: 8px; height: 8px; border-radius: 50%; background: var(--success); }
    
    .container { display: flex; flex-wrap: wrap; gap: 20px; max-width: 1100px; width: 100%; justify-content: center; }
    .card { background: var(--card-bg); border: 1px solid var(--card-border); border-radius: 16px; padding: 20px; box-shadow: 0 12px 24px -6px rgba(0,0,0,0.5); }
    
    .stream-card { flex: 2; min-width: 320px; display: flex; flex-direction: column; align-items: center; }
    .control-card { flex: 1; min-width: 300px; max-width: 440px; }
    
    .view-box { position: relative; width: 100%; min-height: 280px; background: #000; border-radius: 12px; overflow: hidden; display: flex; align-items: center; justify-content: center; border: 1px solid var(--card-border); }
    #stream { width: 100%; height: auto; max-height: 520px; object-fit: contain; display: block; border-radius: 12px; }
    
    .btn-row { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; width: 100%; margin-top: 16px; }
    .btn { background: var(--accent); color: #090d16; font-weight: 700; border: none; padding: 12px 16px; border-radius: 8px; font-size: 0.95rem; cursor: pointer; transition: all 0.2s ease; display: flex; align-items: center; justify-content: center; gap: 8px; }
    .btn:hover { background: var(--accent-hover); color: #fff; transform: translateY(-1px); }
    .btn-danger { background: var(--danger); color: white; }
    .btn-secondary { background: #1e2d4a; color: var(--text-main); }
    .btn-secondary:hover { background: #2a3e66; }
    
    .section-title { color: var(--accent); font-size: 1.1rem; font-weight: 600; margin-bottom: 16px; border-bottom: 1px solid var(--card-border); padding-bottom: 8px; }
    .control-group { margin-bottom: 16px; }
    label { display: flex; justify-content: space-between; font-size: 0.85rem; color: var(--text-sub); margin-bottom: 6px; font-weight: 500; }
    span.val { color: var(--accent); font-weight: 700; }
    
    select, input[type="range"] { width: 100%; padding: 10px 12px; background: #090d16; border: 1px solid var(--card-border); color: var(--text-main); border-radius: 8px; font-size: 0.9rem; outline: none; transition: border-color 0.2s; }
    select:focus { border-color: var(--accent); }
    input[type="range"] { -webkit-appearance: none; height: 6px; background: #1e2d4a; border-radius: 3px; cursor: pointer; border: none; }
    input[type="range"]::-webkit-slider-thumb { -webkit-appearance: none; width: 18px; height: 18px; border-radius: 50%; background: var(--accent); cursor: pointer; border: 2px solid var(--card-bg); }
  </style>
</head>
<body>
  <header>
    <h1>📷 ESP32-CAM Live Webcam</h1>
    <p class="subtitle">HW-411 V1.2 USB-to-TTL Serial Bridge</p>
    <div class="badge" id="badge"><div class="badge-dot"></div> <span id="status">Ready</span></div>
  </header>

  <div class="container">
    <div class="card stream-card">
      <div class="view-box">
        <img id="stream" src="" alt="Stream Offline - Click 'Start Stream' to View">
      </div>
      <div class="btn-row">
        <button class="btn" id="stream-btn" onclick="toggleStream()">▶ Start Stream</button>
        <button class="btn btn-secondary" onclick="captureStill()">📸 Capture Snapshot</button>
      </div>
    </div>

    <div class="card control-card">
      <div class="section-title">Camera & Stream Controls</div>
      
      <div class="control-group">
        <label>Flash LED Brightness <span class="val" id="flash-val">0</span></label>
        <input type="range" id="flash" min="0" max="255" value="0" onchange="updateParam('flash', this.value)">
      </div>

      <div class="control-group">
        <label>Resolution</label>
        <select id="framesize" onchange="updateParam('framesize', this.value)">
          <option value="10">UXGA (1600x1200)</option>
          <option value="9">SXGA (1280x1024)</option>
          <option value="8">XGA (1024x768)</option>
          <option value="7">SVGA (800x600)</option>
          <option value="6" selected>VGA (640x480) - Recommended</option>
          <option value="5">CIF (400x296)</option>
          <option value="4">QVGA (320x240)</option>
        </select>
      </div>

      <div class="control-group">
        <label>JPEG Quality (10=Best, 63=Fastest) <span class="val" id="quality-val">12</span></label>
        <input type="range" id="quality" min="10" max="63" value="12" onchange="updateParam('quality', this.value)">
      </div>

      <div class="control-group">
        <label>Brightness <span class="val" id="brightness-val">0</span></label>
        <input type="range" id="brightness" min="-2" max="2" value="0" onchange="updateParam('brightness', this.value)">
      </div>

      <div class="control-group">
        <label>Contrast <span class="val" id="contrast-val">0</span></label>
        <input type="range" id="contrast" min="-2" max="2" value="0" onchange="updateParam('contrast', this.value)">
      </div>

      <div class="btn-row">
        <button class="btn btn-secondary" onclick="toggleOption('hmirror')">↔ Horizontal Flip</button>
        <button class="btn btn-secondary" onclick="toggleOption('vflip')">↕ Vertical Flip</button>
      </div>
    </div>
  </div>

  <script>
    const streamImg = document.getElementById('stream');
    const streamBtn = document.getElementById('stream-btn');
    const statusText = document.getElementById('status');
    const baseHost = document.location.origin;
    let streamActive = false;
    let hmirrorState = 0;
    let vflipState = 0;

    function toggleStream() {
      if (!streamActive) {
        streamImg.src = `${baseHost}:81/stream`;
        streamBtn.innerText = "⏹ Stop Stream";
        streamBtn.classList.add("btn-danger");
        streamActive = true;
        statusText.innerText = "Live Streaming";
      } else {
        streamImg.src = "";
        streamBtn.innerText = "▶ Start Stream";
        streamBtn.classList.remove("btn-danger");
        streamActive = false;
        statusText.innerText = "Stream Paused";
      }
    }

    function captureStill() {
      window.open(`${baseHost}/capture`, '_blank');
    }

    function updateParam(param, val) {
      if (document.getElementById(`${param}-val`)) {
        document.getElementById(`${param}-val`).innerText = val;
      }
      fetch(`${baseHost}/control?var=${param}&val=${val}`)
        .catch(err => console.error("Control Error:", err));
    }

    function toggleOption(param) {
      if (param === 'hmirror') {
        hmirrorState = hmirrorState === 0 ? 1 : 0;
        updateParam('hmirror', hmirrorState);
      } else if (param === 'vflip') {
        vflipState = vflipState === 0 ? 1 : 0;
        updateParam('vflip', vflipState);
      }
    }
  </script>
</body>
</html>
)rawliteral";

static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}

static esp_err_t capture_handler(httpd_req_t *req) {
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;

  if (flash_brightness > 0) {
    ledcWrite(0, flash_brightness);
    delay(50);
  }

  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("❌ Camera capture failed!");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  if (flash_brightness > 0) {
    ledcWrite(0, 0);
  }

  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=snapshot.jpg");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
  return res;
}

static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char part_buf[128];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) return res;

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("❌ Camera frame capture failed");
      res = ESP_FAIL;
    } else {
      _jpg_buf_len = fb->len;
      _jpg_buf = fb->buf;
    }

    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if (res == ESP_OK) {
      size_t hlen = snprintf(part_buf, 128, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, part_buf, hlen);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }

    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else {
      break;
    }

    if (res != ESP_OK) {
      break;
    }
  }
  return res;
}

static esp_err_t control_handler(httpd_req_t *req) {
  char* buf;
  size_t buf_len;
  char variable[32] = {0,};
  char value[32] = {0,};

  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if (!buf) {
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) == ESP_OK &&
          httpd_query_key_value(buf, "val", value, sizeof(value)) == ESP_OK) {
      } else {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
      }
    }
    free(buf);
  } else {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  int val = atoi(value);
  sensor_t * s = esp_camera_sensor_get();
  int res = 0;

  if (!strcmp(variable, "framesize")) {
    if (s->pixformat == PIXFORMAT_JPEG) res = s->set_framesize(s, (framesize_t)val);
  } else if (!strcmp(variable, "quality")) {
    res = s->set_quality(s, val);
  } else if (!strcmp(variable, "contrast")) {
    res = s->set_contrast(s, val);
  } else if (!strcmp(variable, "brightness")) {
    res = s->set_brightness(s, val);
  } else if (!strcmp(variable, "hmirror")) {
    res = s->set_hmirror(s, val);
  } else if (!strcmp(variable, "vflip")) {
    res = s->set_vflip(s, val);
  } else if (!strcmp(variable, "flash")) {
    flash_brightness = val;
    ledcWrite(0, flash_brightness);
  } else {
    res = -1;
  }

  if (res) {
    return httpd_resp_send_500(req);
  }

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
}

void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t capture_uri = {
    .uri       = "/capture",
    .method    = HTTP_GET,
    .handler   = capture_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t control_uri = {
    .uri       = "/control",
    .method    = HTTP_GET,
    .handler   = control_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t stream_uri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };

  Serial.printf("🌐 Starting Control Server on Port %d...\n", config.server_port);
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &capture_uri);
    httpd_register_uri_handler(camera_httpd, &control_uri);
  }

  config.server_port += 1;
  config.ctrl_port += 1;
  Serial.printf("🎥 Starting Video Stream Server on Port %d...\n", config.server_port);
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\n=======================================================");
  Serial.println(" ESP32-CAM Live Webcam Server (HW-411 V1.2 Bridge)");
  Serial.println("=======================================================");

  ledcSetup(0, 5000, 8);
  ledcAttachPin(FLASH_LED_PIN, 0);
  ledcWrite(0, 0);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    Serial.println("✅ PSRAM Detected! High resolution stream enabled.");
    config.frame_size   = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count     = 2;
  } else {
    Serial.println("⚠️ PSRAM NOT Detected! Using single frame buffer.");
    config.frame_size   = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count     = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("❌ Camera Initialization Failed! Error code: 0x%x\n", err);
    return;
  }
  Serial.println("✅ OV2640 Camera initialized successfully.");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi SSID: ");
  Serial.print(ssid);

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 25) {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Connected to Wi-Fi!");
    Serial.print("📡 Open web browser at: http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n⚠️ Could not connect to Wi-Fi router. Activating Access Point Mode...");
    WiFi.softAP(ap_ssid, ap_password);
    IPAddress apIP = WiFi.softAPIP();
    Serial.print("📡 Connect to AP SSID: ");
    Serial.println(ap_ssid);
    Serial.print("📡 Open web browser at: http://");
    Serial.println(apIP);
  }

  startCameraServer();
}

void loop() {
  delay(10000);
}
