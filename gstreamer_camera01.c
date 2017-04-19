#include <stdio.h>
#include <gst/gst.h>
#include <glib.h>



int
main (int argc, char *argv[])
{

	GstElement *pipeline, *video_source, *video_convert, *video_sink;
	GstBus *bus;  
    GstMessage *msg;
	
	
	gst_init (&argc, &argv);
	
	video_source = gst_element_factory_make ("v4l2src", "video_source");
	video_convert = gst_element_factory_make ("videoconvert", "video_convert");
	video_sink = gst_element_factory_make ("ximagesink", "video_sink");
	
	pipeline = gst_pipeline_new ("test_pipeline");
	
	if (!pipeline || !video_source || !video_convert || !video_sink)
	{
		g_printerr ("Not all element could be created!\n");
		return -1;
	}
	
	g_object_set (G_OBJECT(video_source), "device", "/dev/video0", NULL);
	
	gst_bin_add_many (GST_BIN(pipeline), video_source, video_convert, video_sink, NULL);
	
	if (gst_element_link_many (video_source, video_convert, video_sink, NULL) != TRUE)
	{
		g_printerr ("Element could not be linked\n");
		gst_object_unref (pipeline);
		return -1;
	}
	
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
