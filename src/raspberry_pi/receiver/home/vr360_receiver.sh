#! /bin/sh

if [ -e /dev/ttyACM0 ]; then
    echo "VR View ... Start"
    /home/pi/vr360_view/bin/vr360_view  -q w,_y,_x,_z -t 90 -f 1.0 -r 115 -s 0.02 -a -3 -b 0
else
    echo "3DoF Sensor Not Detect"
fi

