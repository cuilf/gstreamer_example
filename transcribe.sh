#! /bin/bash

set -e 

gst-launch-1.0 -e v4l2src device=/dev/video0 ! avmux_mp4 ! filesink location=video.mp4
#gst-launch-1.0 -e v4l2src device=/dev/video0 ! avimux ! filesink location=video.avi

