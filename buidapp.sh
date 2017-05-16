#########################################################################
# File Name: buidapp.sh
# Author: cuilf
# mail: cuilongfeiii@163.com
# Created Time: Tue 16 May 2017 09:55:36 AM CST
#########################################################################
#!/bin/bash

gcc -Wall $1 -o $2 $(pkg-config --cflags --libs gstreamer-1.0)
