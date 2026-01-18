#!/bin/sh
scripts/flash.sh $1
tio -b 115200 ${2:-/dev/ttyACM0}
