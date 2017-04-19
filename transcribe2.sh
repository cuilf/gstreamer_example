#! /bin/bash
set -e 

#gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,format=RGB,width=320,height=240,framerate=30/1 ! videoconvert ! ximagesink

gst-launch-1.0 -e v4l2src device=/dev/video0 ! video/x-raw,format=RGB,width=640,height=480,framerate=30/1 ! videoconvert ! tee name=t ! queue ! jpegenc ! avimux ! filesink location=video.avi t. ! queue ! ximagesink




