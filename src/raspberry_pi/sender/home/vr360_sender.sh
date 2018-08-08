#! /bin/sh

if [ -e /dev/video0 ]; then
	echo "Camera Data ... Send Start"
	ffmpeg -f v4l2 -input_format h264 -video_size 1920x960 -framerate 30 -i /dev/video0 -vcodec copy -an  -f mpegts udp://192.168.250.3:1234
else
	echo "Camera No Detect"
fi

