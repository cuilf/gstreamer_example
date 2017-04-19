#include <stdio.h>
#include <gst/gst.h>
#include <gst/gstcaps.h>
#include <glib.h>
#include <unistd.h>
#include <pthread.h>

GstElement *pipeline;


void *handler_end()
{
	sleep(5);
	
	gst_element_send_event(pipeline, gst_event_new_eos());
	pthread_exit(0);
	
}

int
main (int argc, char *argv[])
{
	
	GstElement  *video_source, *video_avmux, *file_sink;
	GstBus *bus;  
    GstMessage *msg;
    
    pthread_t thread_end;
    
    
    gst_init(&argc, &argv);
    
    video_source = gst_element_factory_make ("v4l2src", "video_source");
    video_avmux    = gst_element_factory_make ("avmux_mp4", "video_avmux");
    file_sink   = gst_element_factory_make ("filesink", "file_sink");
    
    pipeline     = gst_pipeline_new ("test_pipeline");
    
    if (!pipeline || !video_source || !video_avmux || !file_sink)
	{
		g_printerr ("Not all element could be created!\n");
		return -1;
	}
	
	g_object_set (video_source, "device", "/dev/video0", NULL);
	g_object_set (file_sink, "location", "test.mp4", NULL);
	
	
	gst_bin_add_many (GST_BIN(pipeline), video_source, video_avmux, file_sink, NULL);	
	if (gst_element_link_many (video_source, video_avmux, file_sink, NULL) != TRUE)
	{
		g_printerr ("Element could not be linked\n");
		gst_object_unref (pipeline);
		return -1;
	}
	
	
	gst_element_set_state (pipeline, GST_STATE_PLAYING); 
	
	 
	pthread_create(&thread_end, NULL, handler_end, NULL);
	pthread_join(thread_end, NULL);	
	
	bus = gst_element_get_bus (pipeline);
  	msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
  	
  	/* Parse message */
	if (msg != NULL) 
	{
		GError *err;
		gchar *debug_info;

		switch (GST_MESSAGE_TYPE (msg)) 
		{
		  case GST_MESSAGE_ERROR:
			gst_message_parse_error (msg, &err, &debug_info);
			g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
			g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
			g_clear_error (&err);
			g_free (debug_info);
			break;
		  case GST_MESSAGE_EOS:
			g_print ("End-Of-Stream reached.\n");
			break;
		  default:
			/* We should not reach here because we only asked for ERRORs and EOS */
			g_printerr ("Unexpected message received.\n");
			break;
		}
		gst_message_unref (msg);
	}
	 
	
	
	gst_object_unref (bus);	 
	gst_element_set_state (pipeline, GST_STATE_NULL);  
	gst_object_unref (pipeline);
	
	
	return 0;
}
