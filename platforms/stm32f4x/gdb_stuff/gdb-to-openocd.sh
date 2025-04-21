#!/bin/bash

openocd_scripts_path="/opt/homebrew/Cellar/open-ocd/0.12.0_1/share/openocd/scripts"
debugger_path="/opt/ST/STM32CubeCLT/GNU-tools-for-STM32/bin/"

probes_path=$openocd_scripts_path/interface
probe="stlink.cfg"

boards_path=$openocd_scripts_path/target
board="stm32f4x.cfg"

openocd -f "$probes_path/$probe" -f "$boards_path/$board" &
"$debugger_path/arm-none-eabi-gdb" -ex "target extended-remote localhost:3333" "$1"
