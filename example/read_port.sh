#!/usr/bin/env sh

DEV="/dev/ttyACM0"

if [ ! -e $DEV ]; then
	echo $DEV not exist
	return
fi

sudo stty -F $DEV raw 115200
sudo cat $DEV
