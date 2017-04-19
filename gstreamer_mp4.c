#include <stdio.h>
#include <gst/gst.h>
#include <glib.h>

//gst-launch-1.0 --eos-on-shutdown videotestsrc pattern=snow ! mp4mux ! filesink location=video.mp4

//                              videotest_src_pad    mp4_sink_pad  

int main (int argc, char *argv[])
{
	
	GstElement *pipeline, *videotest_source, *mp4_mux, *file_sink;
	GstPadTemplate *mp4mux_sink_pad_template;
	GstPad *videotest_src_pad;
	GstPad *mp4mux_sink_pad;
	
	GstBus *bus;  
  GstMessage *msg; 
	
	gst_init(&argc, &argv);
	
	
	videotest_source = gst_element_factory_make("videotestsrc", "videotest_source");
	mp4_mux = gst_element_factory_make ("mp4mux", "mp4_mux");
	file_sink = gst_element_factory_make ("filesink", "file_sink");
	
	pipeline = gst_pipeline_new ("test_pipeline");
	
	if (!pipeline || !videotest_source || !mp4_mux || !file_sink)
	{
		g_printerr ("Not all elements could be created.\n");  
    	return -1; 
	}
	
	g_object_set (videotest_source , "pattern", 1, NULL);
	g_object_set (file_sink, "location", "xxxxx.mp4", NULL);
	
	gst_bin_add_many (GST_BIN(pipeline), videotest_source, mp4_mux, file_sink, NULL);
	
	if (gst_element_link(mp4_mux, file_sink) != TRUE)
	{
		g_printerr ("Elements could not be linked.\n");  
		gst_object_unref (pipeline);  
		return -1;  
	}	
	
	mp4mux_sink_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (mp4_mux), "video_%u"); 
	mp4mux_sink_pad = gst_element_request_pad (mp4_mux, mp4mux_sink_pad_template, NULL, NULL);
	g_print ("Obtained request pad %s for audio branch.\n", gst_pad_get_name (mp4mux_sink_pad));
	
	videotest_src_pad = gst_element_get_static_pad (videotest_source, "src");
	g_print ("Obtained request pad %s for audio branch.\n", gst_pad_get_name (videotest_src_pad));
	
	if (gst_pad_link (videotest_src_pad, mp4mux_sink_pad) != GST_PAD_LINK_OK)
	{
		g_printerr ("mp4_mux could not be linked.\n");  
		gst_object_unref (pipeline);  
		return -1;
	}
	
	gst_object_unref (videotest_src_pad);
	
	gst_element_set_state (pipeline, GST_STATE_PLAYING); 
	
	
	bus = gst_element_get_bus (pipeline);  
	
	
	msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS); 
	 
	
	gst_element_release_request_pad(mp4_mux, mp4mux_sink_pad);
	gst_object_unref (mp4mux_sink_pad);
	

	if (msg != NULL)  
		gst_message_unref (msg);  
	gst_object_unref (bus);  
	gst_element_set_state (pipeline, GST_STATE_NULL);  

	gst_object_unref (pipeline);  
	
	
	
	
	
	
	return 0;
}
