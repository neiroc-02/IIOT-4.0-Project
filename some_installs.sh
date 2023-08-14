#!/bin/bash

#These are all the packages you must install for the project.
#Most are for the GUI, mosquitto is what you use to set up the broker.

packages=("mosquitto" "libglfw-dev" "libglfw3" "xorg-dev" "libxcb1-dev")
for str in ${packages[@]}; 
do
	sudo apt install $str
done

githubs=("https://github.com/ocornut/imgui" "https://github.com/ShakaUVM/read")
for str in ${githubs[@]}; 
do
	git clone $str
done

