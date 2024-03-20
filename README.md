# Smart Pill Organizer

Our solution aims to alleviate the struggle of medication adherence by uniting a traditional pillbox with sensors and actuators. The smart pill organizer is able to detect the medication that is being taken and send the information to a cloud service. This information is then used to send reminders to the user and to keep track of the medication inventory.

## General Information

The major benefits of our solution are the automation of the medication intake process, the ability to keep track of the medication inventory and the possibility of sending reminders to the user. This is a problem felt by many individuals all over the globe that often encounter challenges in adhering to their prescribed medication schedules, due to factors such as age, stress, memory loss, etc.
This struggle with medication adherence can lead to detrimental consequences, including ineffective treatments, deteriorating health conditions, and, in severe cases, death.

## Built With

Include an outline of the technologies in the project, such as hardware components (Arduino/Raspberry Pi), operating systems, programming language, database, libraries.

Include links to any related projects (for example, whether this API has corresponding iOS or Android clients), links to online tools related to the application (such as the project web site, the shared file storage).
If you mention something, please provide links.

### Hardware

* [Arduino Uno](https://store.arduino.cc/products/arduino-uno-rev3) - 1 unit - Development board for interacting with the Pixy2 Camera and to send data to the Ubidots STEM Cloud
* [Arduino Nano ESP32 with headers](https://store.arduino.cc/products/nano-esp32-with-headers) - 1 unit - Development board for interacting with Ubidots STEM Cloud and to interact with the Piezo Buzzer, LEDs and RGB Module
* [Pixy2 Camera](https://pixycam.com/pixy2/) - 1 unit - Vision sensor that can detect and track objects based on their color, used to detect the flunctuation of medication inventory.
* [Green LED Alert](https://www.exploringarduino.com/parts/green-led/) - 3 units - green light-emitting diode (LED) connected to the Arduino Nano ESP32 development board used to provide visual feedback to the user when it's time to take their medication.
* [RGB Module](https://arduinomodules.info/ky-016-rgb-full-color-led-module/) - 1 unit - Electronic component connected to the Arduino Nano ESP32 development board, used to provide visual feedback to the user when the Cloud Platform is offline.
* [Piezzo Buzzer](https://www.adafruit.com/product/160) - 1 unit - Electronic component connected to the Arduino Nano ESP32 development board, used to provide auditory feedback to the user when it's time to take their medication.
* [Resistors](https://en.wikipedia.org/wiki/List_of_electronic_color_code_mnemonics) - 3 units - Electronic components used to limit the flow of electric current to the LED Alerts.
* [USB Cable](https://store.arduino.cc/products/usb-2-0-cable-type-a-b) - 1 unit - Connects the Arduino Uno development board to the computer that supplies its' power.
* [USB Cable](https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:powering_pixy#usb-cableconnector) - 1 unit - Connects the Pixy2 Camera to the computer that supplies its' power.
* [USB-C Cable](https://store.arduino.cc/products/usb-cable2in1-type-c) - 1 unit - Connects the Arduino Nano ESP32 development board to the computer that supplies its' power.
* [Pixy IO to Arduino ISP Cable](https://cdn.sparkfun.com/r/345-345/assets/parts/1/2/8/8/0/14678-Pixy2_CMUcam5-05.jpg) - 1 unit - Connects the Pixy2 camera to the Arduino Uno development board so they can communicate.

### Software

* [Arduino Programming Language](https://www.raspberrypi.com/software/) - Programming Language
* [Python](https://openjdk.java.net/) - Programming Language
* [PixyMon](https://pixycam.com/downloads-pixy2/) - Software that enables parameterising the camera
* [Ubidots STEM](https://ubidots.com/stem) - Platform that provides tools and resources for IoT projects, used for Cloud Storage and Web Interface
* [Zapier](https://zapier.com/) - Web-based automation tool that can automate tasks that connect two or more apps together
* [Pixy2 Library](https://pixycam.com/downloads-pixy2/) - 
* [HttpClient Library](https://maven.apache.org/) - Build Tool and Dependency Management

## Getting Started

These instructions will get you a copy of the project up and running on for testing purposes.  

### Assembly Instructions

Describe step-by-step assembly instructions.

When necessary, and especially when wiring is involved, include diagrams/photos.

Step 1. Arduino Nano ESP32 Assembly

Connect the components to the breadboard as shown in the following diagram:
![alt text](ArduinoNanoESP32_Assembly.png)

Description: 
1. Connect each Green LED to pins D3, D4, and D5 on the Arduino board using a 220-ohm resistor for each.  Attach one end of the resistor to the corresponding pin (D3, D4, or D5) using a wire and the other end to the positive (anode) leg of the LED using a jump wire. Connect the negative (cathode) leg of the LED to ground (GND) on the Arduino board using a jump wire.
2. Connect the RGB LED module to pin D2. Connect the pin labeled "R" on the LED to the pin D2 using a jump wire. Connect the Common (Cathode-) pin to ground (GND) on the Arduino board using a jump wire.
3. Connect the Piezzo Buzzer to pin D12. Connect the positive (anode) leg of the buzzer to the pin D12 using a jump wire and connect the negative (cathode) leg of the buzzer to the ground (GND) on the Arduino board using a jump wire.
4. Connect the Arduino using a USB-C cable to the computer to supply its' power.

Step 2. Arduino Uno Assembly 
1. Connect the Pixy2 to the the Arduino Uno using a Pixy IO to Arduino ISP Cable.
2. Connect both of them using a USB Cable to the computer to supply its' power.

### Software Prerequisites

In this section include detailed instructions for installing additiona software the application is dependent upon (such as PostgreSQL database, for example).

```
installation command
```

### Installation

Give step-by-step instructions on building and running the application on the testing environment. 

Describe the step.

```
Give the command example
```

And repeat.

```
until finished
```

You can also add screenshots to show expected results, when relevant.

### Testing

Explain how to run the tests for this system.

Give users explicit instructions on how to run all necessary tests. 

Explain what these tests do and why

```
Give an example command
```

## Demo

Give a tour of the best features of the application.
Add screenshots when relevant.

## Deployment

Add additional notes about how to deploy this on a host or a cloud provider.

Mention virtualization/container tools and commands.

```
Give an example command
```

Provide instructions for connecting to servers and tell clients how to obtain necessary permissions.

## Additional Information

### Authors

* **Billie Thompson** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

### Versioning

We use [SemVer](http://semver.org/) for versioning. 
For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

### License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

### Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

### Acknowledgments

* Hat tip to anyone whose code was used
* Inspiration
* etc




...

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
* install the board for the Arduino Nano ESP32 by going to Tools ➜ Board ➜ Boards Manager... and once in the Boards Manager, search for Arduino ESP32 Boards by Arduino.

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