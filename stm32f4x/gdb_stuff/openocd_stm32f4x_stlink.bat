@echo off

SET "openocd_exe=C:\Users\yt75534\AppData\Local\OpenOCD-20231002-0.12.0\bin\openocd.exe"
SET "openocd_scripts_path=C:\Users\yt75534\AppData\Local\OpenOCD-20231002-0.12.0\share\openocd\scripts"
SET "probes_path=%openocd_scripts_path%\interface"
SET "probe=stlink.cfg"
SET "boards_path=%openocd_scripts_path%\target"
SET "board=stm32f4x.cfg"
"%openocd_exe%" "-f" "%probes_path%\%probe%" "-f" "%boards_path%\%board%"
