#! /bin/sh

if [ -e /dev/ttyACM0 ]; then
    echo "VR View ... Start"
    /home/pi/vr360_view/bin/vr360_view  -q w,_y,_x,_z -t 90 -f 10.0 -r 90 -s 0.02
else
    echo "3DoF Sensor Not Detect"
fi

