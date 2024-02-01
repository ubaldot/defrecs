#!bin/bash

openocd_scripts_path="/opt/homebrew/Cellar/open-ocd/0.12.0_1/share/openocd/scripts"

probes_path=$openocd_scripts_path/interface
probe="stlink.cfg"

boards_path=$openocd_scripts_path/target
board="stm32f4x.cfg"

openocd -f "$probes_path/$probe" -f "$boards_path/$board" -
