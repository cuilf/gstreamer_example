#include <stdio.h>
#include <gst/gst.h>
#include <gst/gstcaps.h>
#include <glib.h>
#include <unistd.h>
#include <pthread.h>
/*
gst-launch-1.0 -e v4l2src device=/dev/video0 ! video/x-raw,format=RGB,width=640,height=480,framerate=30/1 !
 videoconvert ! tee name=t ! queue ! jpegenc ! avimux ! filesink location=video.avi t. ! queue ! ximagesink
*/
typedef struct _appData{

	GstElement *pipeline;
	GstElement *video_source, *video_filter, *video_convert, *tee, *file_queue, *ximage_queue, *video_jpegenc, *video_avimux, *file_sink, *ximage_sink;
	
	 	
}appData;


int init_pipeline (int argc, char **argv, appData *data)
{
	GstCaps *caps;
	gst_init (&argc, &argv);
	
	data->video_source = gst_element_factory_make ("v4l2src", "video_source");
	data->video_filter = gst_element_factory_make ("capsfilter", "video_filter");
	data->video_convert = gst_element_factory_make ("videoconvert", "video_convert");
	data->tee = gst_element_factory_make ("tee", "tee");
	data->file_queue = gst_element_factory_make ("queue", "file_queue");
	data->ximage_queue = gst_element_factory_make ("queue", "ximage_queue");
	data->video_jpegenc = gst_element_factory_make ("jpegenc", "video_jpegenc");
	data->video_avimux = gst_element_factory_make ("avimux", "video_avimux");
	data->file_sink = gst_element_factory_make ("filesink", "file_sink");
	data->ximage_sink = gst_element_factory_make ("ximagesink", "ximage_sink");
	
	data->pipeline = gst_pipeline_new ("new-pipeline");
	
	if (!(data->pipeline && data->video_source && data->video_filter && data->video_convert && data->tee && data->file_queue  
		&& data->ximage_queue && data->video_jpegenc && data->video_avimux && data->file_sink && data->ximage_sink))
	{
		g_printerr ("Not all element be created \n");
		return -1;
	}
	
	g_object_set (data->video_source, "device", "/dev/video0", NULL);
	g_object_set (data->file_sink, "location", "video_test.avi", NULL);
	
	caps = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "RGB",
          "width", G_TYPE_INT, 640,
          "height", G_TYPE_INT, 480,
          "framerate", GST_TYPE_FRACTION, 30, 1,
          NULL);
    g_object_set (data->video_filter, "caps", caps, NULL);
    
    gst_bin_add_many (GST_BIN(data->pipeline), data->video_source, data->video_filter, data->video_convert, data->tee, data->file_queue, 
    		data->ximage_queue, data->video_jpegenc, data->video_avimux, data->file_sink, data->ximage_sink, NULL);
    		
    if (gst_element_link_many (data->video_source, data->video_filter, data->video_convert, data->tee, data->file_queue, data->video_jpegenc, 
    	data->video_avimux, data->file_sink, NULL) != TRUE ||
    	gst_element_link_many (data->tee, data->ximage_queue, data->ximage_sink, NULL) != TRUE)
    {
    	g_printerr ("Element could not be linked\n");
    	return -1;
    }	
	
	return 0;
	
}

void *handler_end(void *arg)
{
	appData *data = (appData*)arg;
	
	sleep(5);
	
	gst_element_send_event(data->pipeline, gst_event_new_eos());																																																																																																																																																										
	pthread_exit(0);
}

void destroy_pipeline(appData *data)
{
	GstBus *bus;
    GstMessage *msg;
	bus = gst_element_get_bus (data->pipeline);
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
	gst_element_set_state (data->pipeline, GST_STATE_NULL);  
	gst_object_unref (data->pipeline);
}

int
main (int argc, char *argv[])
{
	
	appData data;
	pthread_t thread_end;
	
	if (init_pipeline (argc, argv, &data) != 0)
	{
		g_printerr ("init_pipeline failed \n");
		return -1;
	}
	
	gst_element_set_state (data.pipeline, GST_STATE_PLAYING); 
	
	pthread_create (&thread_end, NULL, handler_end, &data);
	
	
	destroy_pipeline(&data);
	
	
	
	
	
	return 0;
}
