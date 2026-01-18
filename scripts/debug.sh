#!/bin/sh
openocd \
  -f interface/stlink.cfg \
  -f target/stm32f4x.cfg
