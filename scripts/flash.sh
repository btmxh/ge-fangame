#!/bin/sh
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program ${1:-build/stm/ge-app/Debug/ge-app} verify reset exit"
