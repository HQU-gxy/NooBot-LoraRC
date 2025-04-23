# NooBot-LoraRC
This is the remote controller terminal software of NooBot project.
It communicates with the bot via LoRa, sending the speed controlling message
and receiving, displayig the status of the bot.

## Hardware
- `EByte E62-433T30D` UART LoRa Module
- `ESP32-S2` as controller
- Single cell Li-ion Battery
- `TP4056` battery charger chip
- `BMI088` IMU for body-sensing controlling mode
- `PCF8574` GPIO expander for buttons
- `ST7789` TFT LCD screen

## Software
This project uses the ESP-IDF and Arduino framework.  

### Build
0. Install the ESP-IDF environment.

1. Clone the repository:
```bash
git clone https://github.com/HQU-gxy/NooBot-LoraRC.git
```

2. Change the directory to the project and export ESP-IDF path:
```bash
cd footSensor
. path/to/idf/export.sh 
```

3. Build the project:
```bash
idf.py build
```

4. Flash the executable to the chip:
```bash
idf.py -p /dev/ttyUSB0 flash # Change the port to the one you are using
```


