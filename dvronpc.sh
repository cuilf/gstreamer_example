#########################################################################
# File Name: dvronpc.sh
# Author: cuilf
# mail: cuilongfeiii@163.com
# Created Time: Tue 16 May 2017 09:17:59 AM CST
#########################################################################
#!/bin/bash

#gst-launch-1.0 v4l2src device=/dev/video0 ! 'video/x-raw, format=RGB,width=1280,height=720,framerate=30/1' ! videoconvert ! ximagesink 
gst-launch-1.0 -e v4l2src device=/dev/video0  ! 'video/x-raw, format=(string)RGB, width=(int)1280, height=(int)720, framerate=30/1' !  videoconvert ! ximagesink
