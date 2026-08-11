# ESP32-CAM Live Wi-Fi Webcam with HW-411 V1.2 USB-to-TTL Converter

This project provides complete code, wiring diagrams, and step-by-step flashing instructions to transform an **ESP32-CAM (AI-Thinker module)** into a **high-speed Wi-Fi live streaming webcam** using the **HW-411 V1.2 USB-to-TTL serial converter**.

---

## 📷 Features

- 🎥 **Real-time MJPEG Video Streaming** over local Wi-Fi network.
- 📱 **Modern Interactive Web Dashboard**: Accessible from any Smartphone, Tablet, or PC.
- ⚙️ **On-the-Fly Camera Controls**: Adjust Resolution (UXGA to QVGA), Quality, Brightness, Contrast, H-Flip, and V-Flip without restarting.
- 💡 **Flash LED Control**: Dimming brightness control for the onboard high-power flash LED (GPIO 4).
- 📶 **Auto Access Point (AP) Fallback**: If home Wi-Fi is unavailable or credentials fail, it creates an AP hotspot (`ESP32-CAM-Webcam`).
- ⚡ **Brownout Protection**: Includes software brownout suppression to prevent power reset loops.

---

## 🛠️ Required Hardware

1. **ESP32-CAM Board** (AI-Thinker version with OV2640 camera module).
2. **HW-411 V1.2 USB-to-TTL Converter** (FT232RL / CH340 / CP2102 based).
3. **Female-to-Female Jumper Wires** (at least 5 wires).
4. **USB Micro / Mini Cable** (Data capable).
5. *(Optional but Recommended)* **100µF – 470µF Capacitor** across 5V and GND for power decoupling.

---

## 🔌 Circuit & Wiring Table

> **IMPORTANT**: Set the voltage jumper on the **HW-411 V1.2** to **5V**. ESP32-CAM requires 5V power input for Wi-Fi and camera stability.

| HW-411 V1.2 USB-to-TTL Pin | ESP32-CAM Pin | Purpose / Description |
| :--- | :--- | :--- |
| **VCC (5V)** | **5V** | Main 5V power supply to ESP32-CAM |
| **GND** | **GND** | Ground reference (Connect both GNDs) |
| **TXD** | **U0R (GPIO 3)** | Converter TX → ESP32 RX |
| **RXD** | **U0T (GPIO 1)** | Converter RX → ESP32 TX |
| **GND (or Jumper wire)** | **GPIO 0** | **BRIDGED TO GND DURING FLASHING ONLY!** |

> ⚠️ **CRITICAL FLASHING RULE**: **GPIO 0** MUST be connected to **GND** when powering up or resetting the board to enter **UART Flashing Mode**. Once upload completes, **DISCONNECT GPIO 0 from GND** and reset the board to boot normally!

---

## 💻 Software Setup & Flashing Instructions

### Option A: Arduino IDE

1. **Install ESP32 Board Package**:
   - Open Arduino IDE → `File` → `Preferences`.
   - Add URL to *Additional Boards Manager URLs*:
     `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Go to `Tools` → `Board` → `Boards Manager`, search for **esp32** and install it.

2. **Open the Sketch**:
   - Open [`esp32cam_hw411_server.ino`](esp32cam_hw411_server.ino).

3. **Configure Wi-Fi Credentials**:
   - Change `ssid` and `password` on lines 34-35 to match your Wi-Fi network:
     ```cpp
     const char* ssid     = "Your_WiFi_Name";
     const char* password = "Your_WiFi_Password";
     ```

4. **Select Board Settings** (`Tools` Menu):
   - **Board**: `AI Thinker ESP32-CAM`
   - **CPU Frequency**: `240MHz (WiFi/BT)`
   - **Flash Frequency**: `80MHz`
   - **Flash Mode**: `QIO`
   - **Partition Scheme**: `Huge APP (3MB No OTA/1MB SPIFFS)`
   - **PSRAM**: `Enabled`
   - **Port**: Select the COM port corresponding to your HW-411 V1.2 converter.

5. **Upload Code**:
   - Ensure **GPIO 0 is bridged to GND**.
   - Plug the HW-411 V1.2 into your PC USB port.
   - Click **Upload**.
   - *If stuck on `Connecting......`*, press the **RST button** on the back of the ESP32-CAM module.

6. **Run Webcam**:
   - After seeing `Done uploading`:
     1. Unplug USB or disconnect **GPIO 0 from GND**.
     2. Reconnect USB (or press **RST** button).
     3. Open **Serial Monitor** at **115200 baud**.
     4. Note down the IP address displayed (e.g., `http://192.168.1.150`).

---

### Option B: PlatformIO (VS Code)

1. Open the project folder [`esp32cam-hw411-stream`](.) in PlatformIO.
2. Edit `src/main.cpp` with your Wi-Fi SSID and Password.
3. Bridge **GPIO 0 to GND**.
4. Click **PlatformIO: Upload**.
5. Disconnect **GPIO 0 from GND** and press **RST**.
6. Open **PlatformIO: Serial Monitor** (115200 baud).

---

## 🌐 How to Use the Webcam

1. Make sure your computer/phone is connected to the same Wi-Fi network.
2. Open Chrome, Safari, Firefox, or Edge browser and go to `http://<ESP32-CAM-IP-ADDRESS>`.
3. Click **▶ Start Stream** to view live MJPEG video.
4. Use the controls on the right panel to change resolution, toggle flash LED, or capture high-resolution snapshots!

---

## ⚡ Troubleshooting & Tips

- **Brownout Detector Reset / Reboots during Wi-Fi connection**:
  - Wi-Fi draws up to 500mA peak current. USB ports or cheap cables can drop voltage below 4.5V.
  - Software fix is included (`WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);`).
  - Hardware fix: Add a 100µF to 470µF electrolytic capacitor between the **5V** and **GND** pins of the ESP32-CAM.

- **`Failed to connect to ESP32: Timed out waiting for packet header`**:
  - Check TX to RX (HW-411 TX -> ESP RX / HW-411 RX -> ESP TX).
  - Check that **GPIO 0 is firmly wired to GND**.
  - Hold down the **RST button** when `Connecting...` appears in the console, then release it.

- **`Camera init failed with error 0x20001`**:
  - Ribbon cable of the OV2640 camera is loose. Gently unclip the connector, re-seat the camera ribbon cable, and lock it back.
