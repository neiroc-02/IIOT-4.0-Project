# IIOT-4.0-Project

This project is sponsored by the National Science Foundation to bring exposure to concepts regarding Fourth Industrial Revolution with minimal cost. This is specifically designed for demonstrations at the High School level and more in depth training at the college level. Additionally, the demo serves as a great starting point for anyone interested in learning on a functioning prototype.

This repo will contain steps to set up a system to create an Internet of Things (IOT) using Arduino ESP32s as clients and a Raspberry Pi as an MQTT Broker. It will also include a GUI designed for this project using dearImgui. This repo will describe the steps and resources needed to recreate this demo. This will include any necessary installs of libraries along with links to resources we found helpful in the creation of this demo.

## Some Vocab

In Progress...

## Mosquitto and Raspberry Pi Broker

![th](https://github.com/neiroc-02/IIOT-4.0-Project/assets/113735504/59e3cf17-bcc4-4483-960a-695b1663d10c)

Helpful Link -> https://www.switchdoc.com/2018/02/tutorial-installing-and-testing-mosquitto-mqtt-on-raspberry-pi/

The Raspberry Pi will serve as the broker that controls and distributes the data between the Arduino and the GUI (and a SCADA system like Ignition as well). This will allow for data to be sent between devices. Some important commands to know are as follows:

  ```mosquitto or sudo /etc/init.d/mosquitto start ``` - Starts the Broker
  
  ```sudo /etc/init.d/mosquitto stop               ``` - Stops the Broker
  
  ```mosquitto_sub -d -t "{namespace}"             ``` - Subscribes you to a namespace 
  
  ```mosquitto_pub -d -t "{namespace}" -m "{data}" ``` - Sends data to the namespace specified

This serves as the heart of the demo. Mosquitto (MQTT) allows for the communication between smart devices. This uses the unified namespace to set a naming scheme between different data sets. If you would like to send a message to line1/thing1/out/message from the terminal, the mosquitto command would equate to…

```mosquitto_pub -d -t “line1/thing1/out/message” -m “Hello World!”```

Before you use the broker, you must edit your /etc/mosquitto/mosquitto.conf file. In the end, your mosquitto.conf file should look like this...

```
pid_file /run/mosquitto/mosquitto.pid

persistence true
persistence_location /var/lib/mosquitto/

log_dest topic

log_type error
log_type warning
log_type notice
log_type information

connection_messages true
log_timestamp true
allow_anonymous true
listener 1883

include_dir /etc/mosquitto/conf.d
```

Our Arduino and GUI code utilizes this functionality to communicate between everything involved in our network. The applications become clearer as you continue to read through our demo. 

### The Router

In order to run the project, you must have a router to avoid firewalls. The way we chose to do this was to buy a cheap router and configure it to this setting:

Name:     ```IT4 Project```

Password: ```IOT12345```

The Raspberry Pi must also be configured with a static IP address. You can either do that through the router or dhcpcd.conf file. A helpful article for the second method is included below:

Helpful Link -> https://www.tomshardware.com/how-to/static-ip-raspberry-pi

### Crontab

Helpful Link -> https://www.tomshardware.com/how-to/run-script-at-boot-raspberry-pi

Another useful tip, if you would like the broker to start on boot, follow these steps.
1. Type this in your terminal:       ```crontab -e```
2. Inside your crontab, type this:   ```@reboot mosquitto```
3. Exit and save the crontab and then the broker should start on boot.

## The ESP32

![th](https://github.com/neiroc-02/IIOT-4.0-Project/assets/113735504/6c1db13e-7317-4324-9925-5e29fd4c2c71)

Helpful Link -> https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/

This demo utilizes the Arduino ESP32 since it has WiFi capabilities that allow it to connect to a network where smart devices can communicate with the broker. This library uses Wifi.h to connect to the same network as the broker. This allows the ESP32 to communicate and send its data through MQTT to the broker. 

## The GUI

DearImgui Github -> https://github.com/ocornut/imgui

The GUI serves to display the data from the broker in an easy to visualize manner. This displays the power of Internet of Things systems without having to learn a SCADA system like Ignition. DearImgui is a C++ based Graphical User Interface Library that allows for the creation of windows that can display information or graphs. This allows us to display all the data being circulated in our network between the Arduino, Pi, Ignition (maybe), and the GUI itself.

![GUI](https://github.com/neiroc-02/IIOT-4.0-Project/assets/113735504/7854474c-ead8-4735-a595-4c0e23f0556e)

### Read Library

Read.h Github -> https://github.com/ShakaUVM/read

The Read library is used to simplify IO and make parsing mosquitto messages easier. The read library combines cin and vetting to allow us to write simpler code in fewer lines.

## The PCB

We are currently updating the PCB after Version 1.0. There were some issues in the build and we would like to make sure we publish one here that works and is functional. We appreciate your patience.
