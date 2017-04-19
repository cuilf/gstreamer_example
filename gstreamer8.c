#include <stdio.h>
#include <gst/gst.h>
#include <glib.h>
#include <string.h>

#define CHUNK_SIZE 1024   /* Amount of bytes we are sending in each buffer */  
#define SAMPLE_RATE 44100 /* Samples per second we are sending */  
#define AUDIO_CAPS "audio/x-raw-int,channels=1,rate=%d,signed=(boolean)true,width=16,depth=16,endianness=BYTE_ORDER" 

typedef struct _CustomData{
	GstElement *pipeline, *app_source, *tee, *audio_queue, *audio_convert1, *audio_convert2, *audio_resample,*audio_sink;
	GstElement *video_queue, *visual, *video_convert, *video_sink;
	GstElement *app_queue, *app_sink;
	
	guint num_sample;
	gfloat a, b, c, d;
	
	guint sourceid;
	GMainLoop *main_loop;
	
}CustomData;

static gboolean push_data (CustomData *data)
{
	GstBuffer *buffer;
	GstFlowReturn ret;
	int i;
	gint16 *raw;
	gint num_sample = CHUNK_SIZE / 2;
	gfloat freq;
	
	buffer = gst_buffer_new_and_alloc (CHUNK_SIZE); 
	
	GST_BUFFER_TIMESTAMP (buffer) = gst_util_uint64_scale (data->num_sample, GST_SECOND, SAMPLE_RATE);
	GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale (CHUNK_SIZE, GST_SECOND, SAMPLE_RATE); 

	raw = (gint16 *) GST_BUFFER_DATA (buffer);  
	
	data->c += data->d;  
	data->d -= data->c / 1000;  
	freq = 1100 + 11000 * data->d; 
	
	for (i = 0; i < num_sample; i++) 
	{  
		data->a += data->b;  
		data->b -= data->a / freq;  
		raw[i] = (gint16)(5500 * data->a);  
	}  
	data->num_sample += num_sample;  


	g_signal_emit_by_name (data->app_source, "push-buffer", buffer, &ret);  


	gst_buffer_unref (buffer);  

	if (ret != GST_FLOW_OK)
	{  
		return FALSE;  
	}  

	return TRUE; 
	
	
}


static void start_feed(GstElement *source, guint size, CustomData *data)
{
	if (data->sourceid == 0)
	{
		g_print ("start feeding\n");
		data->sourceid = g_idle_add ((GSourceFunc)push_data, data);
	}
}

static void stop_feed (GstElement *source, CustomData *data) 
{  
	if (data->sourceid != 0)
	{  
		g_print ("Stop feeding\n");  
		g_source_remove (data->sourceid);  
		data->sourceid = 0;  
	}  
}

static void new_buffer (GstElement *sink, CustomData *data) 
{  
	GstBuffer *buffer;  
 
	g_signal_emit_by_name (sink, "pull-buffer", &buffer);  
	if (buffer)
	{  
	 
		g_print ("*");  
		gst_buffer_unref (buffer);  
	}  
} 


static void error_cb (GstBus *bus, GstMessage *msg, CustomData *data) 
{  
	GError *err;  
	gchar *debug_info;  

	/* Print error details on the screen */  
	gst_message_parse_error (msg, &err, &debug_info);  
	g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);  
	g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");  
	g_clear_error (&err);  
	g_free (debug_info);  

	g_main_loop_quit (data->main_loop);  
} 
int main(int argc, char *argv[])
{
	CustomData data;
	GstBus *bus;  
	GstMessage *msg;  
	GstPadTemplate *tee_src_pad_template;
	GstPad *tee_audio_pad, *tee_video_pad, *tee_app_pad;
	GstPad *queue_audio_pad, *queue_video_pad, *queue_app_pad; 
	
	gchar *audio_caps_text;
	GstCaps *audio_caps;
	
	memset(&data, 0, sizeof (data));
	data.b = 1;
	data.d = 1;
	
	gst_init(&argc, &argv);

	 
	data.app_source = gst_element_factory_make("appsrc", "app_source");
	data.tee = gst_element_factory_make("tee", "tee");
	data.audio_queue = gst_element_factory_make("queue", "audio_queue");
	data.audio_convert1 = gst_element_factory_make("audioconvert", "audio_convert1");
	data.audio_convert2 = gst_element_factory_make("audioconvert", "audio_convert2");
	data.audio_resample = gst_element_factory_make("audioresample", "audio_resample");	
	data.audio_sink = gst_element_factory_make("autoaudiosink", "audio_sink");
	data.video_queue = gst_element_factory_make("queue", "video_queue");
	data.visual = gst_element_factory_make("wavescope", "visual");
	data.video_sink   = gst_element_factory_make("autovideosink", "video_sink");
	data.app_queue = gst_element_factory_make("queue", "app_queue");
	data.app_sink = gst_element_factory_make ("appsink", "app_sink");

	data.pipeline = gst_pipeline_new("test_pipelssssss设置ine");

	if (!data.pipeline || !data.app_source || !data.tee || !data.audio_queue || !data.audio_convert1 || !data.audio_convert1 || !data.audio_resample || !data.audio_sink || \
		!data.video_queue || !data.visual || !data.video_convert || !data.video_sink || !data.app_queue || ! data.app_sink) 
	{  
	data.video_convert = gst_element_factory_make("videoconvert", "video_convert");	
		g_printerr ("Not all elements could be created.\n");  
		return -1;  
	}  
	
	
	g_object_set(data.visual, "shader", 0, "style", 3, NULL);
	
	//设置appsrc的caps属性
	audio_caps_text = g_strdup_printf (AUDIO_CAPS, SAMPLE_RATE);
	audio_caps = gst_caps_from_string(audio_caps_text);
	g_object_set(data.app_source, "caps", audio_caps, NULL);
	
	//添加信号和回调函数关联，数据不足或者要满的时候调用函数。
	g_signal_connect (data.app_source, "need-data", G_CALLBACK(start_feed), &data);
	g_signal_connect (data.app_source, "enough-data", G_CALLBACK(stop_feed), &data);
	
	//设置audiosink属性
	g_object_set (data.app_sink, "emit-signals", TRUE, "caps", audio_caps, NULL);
	
	g_signal_connect (data.app_sink, "new_buffer", G_CALLBACK(new_buffer), &data);
	
	gst_caps_unref(audio_caps);
	g_free (audio_caps_text);
	
	
	
	

	gst_bin_add_many(GST_BIN(data.pipeline), data.app_source, data.tee, data.audio_queue, data.audio_convert1, data.audio_resample, \
	data.audio_sink, data.video_queue, data.audio_convert2, data.visual, data.video_sink, data.video_convert, data.app_queue, data.app_sink, NULL);

	if (gst_element_link(data.app_source, data.tee) != TRUE ||
		gst_element_link_many(data.audio_queue, data.audio_convert1, data.audio_resample, data.audio_sink, NULL) != TRUE || 
		gst_element_link_many(data.video_queue, data.audio_convert2, data.visual, data.video_convert, data.video_sink, NULL) != TRUE ||
		gst_element_link_many(data.app_queue, data.app_sink, NULL) != TRUE )
	{ 
		g_printerr ("Elements could not be linked.\n");  
		gst_object_unref (data.pipeline);  
		return -1;  	
	}
	
	//获取tee的输出pad模板
	tee_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (data.tee), "src_%u");  
	
	//请求输出pad模板
	tee_audio_pad = gst_element_request_pad (data.tee, tee_src_pad_template, NULL, NULL);  
	g_print ("Obtained request pad %s for audio branch.\n", gst_pad_get_name (tee_audio_pad));  
	//获取静态pad模板
	queue_audio_pad = gst_element_get_static_pad (data.audio_queue, "sink");  
	
	
	tee_video_pad = gst_element_request_pad (data.tee, tee_src_pad_template, NULL, NULL);  
	g_print ("Obtained request pad %s for video branch.\n", gst_pad_get_name (tee_video_pad));  
	queue_video_pad = gst_element_get_static_pad (data.video_queue, "sink");  
	
	tee_app_pad = gst_element_request_pad (data.tee, tee_src_pad_template, NULL, NULL);  
	g_print ("Obtained request pad %s for app branch.\n", gst_pad_get_name (tee_app_pad));  
	queue_app_pad = gst_element_get_static_pad (data.app_queue, "sink");  
	
	
	//连接tee的输出pad和queue_audio、queue_video的输入pad
	if (gst_pad_link (tee_audio_pad, queue_audio_pad) != GST_PAD_LINK_OK ||  
	    gst_pad_link (tee_video_pad, queue_video_pad) != GST_PAD_LINK_OK ||
	    gst_pad_link (tee_app_pad, queue_app_pad) != GST_PAD_LINK_OK) 
	{  
		g_printerr ("Tee could not be linked.\n");  
		gst_object_unref (data.pipeline);  
		return -1;  
	}  
	gst_object_unref (queue_audio_pad);  
	gst_object_unref (queue_video_pad);  
	gst_object_unref (queue_app_pad);
	

	

	//监听bus
	bus = gst_element_get_bus (data.pipeline); 
	gst_bus_add_signal_watch (bus);
	g_signal_connect (G_OBJECT(bus), "message::error", (GCallback)error_cb, &data);
	gst_object_unref (bus);
	 
	 
	gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
	
  	//msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);  
    
    data.main_loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (data.main_loop);
    
    
    
	gst_element_release_request_pad (data.tee, tee_audio_pad);  
	gst_element_release_request_pad (data.tee, tee_video_pad);  
	gst_element_release_request_pad (data.tee, tee_app_pad);
	gst_object_unref (tee_audio_pad);  
	gst_object_unref (tee_video_pad); 
	gst_object_unref (tee_app_pad);

	
	gst_element_set_state (data.pipeline, GST_STATE_NULL);  

	gst_object_unref (data.pipeline);  
	return 0;  
	


}
