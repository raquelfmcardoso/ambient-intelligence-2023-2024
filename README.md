# Ambient Intelligence Project 2023-2024

## Required Libraries

The following libraries are required for this project:

### Python 3.11
Execute the following commands:

```console  
pip install pyserial
pip install requests
```

### Arduino Uno

* download the library for the Pixy2 camera [here](https://github.com/charmedlabs/pixy2/raw/master/releases/arduino/arduino_pixy2-1.0.3.zip) and in Arduino IDE install it by going to Sketch ➜ Include Library ➜ Add .ZIP Library… and choosing the downloaded file.

### Arduino Nano ESP32

* install the board for the Arduino Nano ESP32 by going to Tools ➜ Board ➜ Boards Manager... and once in the Boards Manager, search for Arduino ESP32 Boards by Arduino.
* download the library HttpClient by going to Sketch ➜ Include Library ➜ Manage Libraries... and once in the Library Manager, search for HttpClient by Adrian McEwen.

## Prerequisites

Before you begin, ensure you have...:

* All libraries were downloaded.
* In the file ```smart_pill_organizer/smart_pill_organizer.ino``` replaced the WIFI_SSID and WIFI_PASSWORD fields with the credentials of your Wi-Fi network.
* Ensure that in the file `post_request.py`, the COM port is correctly set to match the one your Arduino Uno is using.
* Ensure that in the file `get_request.py`, the COM port is correctly set to match the one your Arduino Nano ESP32 is using.
* Ensure that in the file `testing/ubidots_down_test.py`, the COM port is correctly set to match the one your Arduino Nano ESP32 is using.

## Ubidots

To access Ubidots, follow these steps:

1. Go [here](https://stem.ubidots.com/accounts/signin/) and log in with the following credentials:
    * Username: `smartpillorganizer_AmI05`
    * Password: `jfv9JhOWNgG9viyj4Blo`
2. Our web interface is located on Data ➜ Dashboards. In the dashboard you are able to see the current inventory, inventory history and insert medication's prescription time.
3. To check variables values go to Devices ➜ Devices ➜ Click on `smart_pill_organizer`

## Zapier

1. Go [here](https://zapier.com/app/login) and log in with the following credentials:
    * Username: `smartpillbox.amia05@gmail.com`
    * Password: `EmpdGpzaal85NslXFGyb`
2. The zap that allows the automation of emails can be accessed through: Starting at Home ➜ Zaps ➜ Click on `Smart Pill Organizer Email Automation`. 
3. The zap can also be edited by clicking on the [link](https://zapier.com/editor/231593518/published).
4. If you wish to add your email to the receivers lists, on the zap (link above), click on `2. Send Email in Gmail` and add the desired email clicking on Step details ➜ Bcc.


## Usage
To use the Smart Pill Organizer, follow these steps:

1. Open the file `camera_detection/camera_detection.ino` in Arduino IDE, select the Arduino Uno board and upload it.
2. Open the file `smart_pill_organizer/smart_pill_organizer.ino` in Arduino IDE, select the Arduino Nano ESP32 board, change the Wi-Fi credentials and upload it onto the board.
3. Close the Arduino IDE.
4. Open the file `post_request.py` in your favorite IDE, change the COM Port details accordingly and start to run it.
5. Open the file `get_request.py` in your favorite IDE, change the COM Port details accordingly and start to run it.
6. Interact with the Smart Pill Organizer.