#!/bin/bash

mosquitto&

cd /usr/local/ignition && sudo ./ignition.sh start

cd ~/IIOT-4.0-Project/GUI && ./IOT_GUI

