

#include <stdio.h>
#include <gst/gst.h>
#include <glib.h>  

int main(int argc, char* argv[])
{
	GstElement* pipeline;
	GstBus *bus;
	GstMessage *msg;

	gst_init(&argc, &argv);

	pipeline = gst_parse_launch ("playbin uri=file://</home/cuilf/gstreamer/bowl1.mp4>", NULL);   

	 gst_element_set_state (pipeline, GST_STATE_PLAYING); 

	bus = gst_element_get_bus (pipeline);  
	msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS); 

	if (msg != NULL)  
		gst_message_unref (msg);  
	gst_object_unref (bus);  
	gst_element_set_state (pipeline, GST_STATE_NULL);  
	gst_object_unref (pipeline); 

return 0;   
	
}

