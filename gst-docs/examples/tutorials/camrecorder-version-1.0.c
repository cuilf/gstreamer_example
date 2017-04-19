/* camrecorder version 1.0 */
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>
/* 定义图片文件名及其保存文件夹 */
#define SAVE_FOLDER_DEFAULT
"/home/semper/Desktop"
#define PHOTO_NAME_DEFAULT
"Picture"
#define PHOTO_NAME_SUFFIX_DEFAULT ".jpg"
/* 定义视频文件名 */
#define VIDEO_NAME_DEFAULT
"Video"
#define VIDEO_NAME_SUFFIX_DEFAULT ".avi"
#define VIDEO_SRC "v4l2src"
#define VIDEO_SINK "xvimagesink"


/* 定义整个应用所需的变量的结构 */
typedef struct
{
	GstElement *pipeline;
	GtkWidget *screen, *button;
	guint buffer_cb_id;
	gboolean record_state;
} AppData;

/* 从摄像源创建一个 jpeg 文件 */
static gboolean create_jpeg(unsigned char *buffer);
/* 从摄像源创建一个 avi 文件 */
static gboolean create_avi(AppData *appdata);


/* 此函数用于创建文件名,如 picture1, picture2... video1, video2... */
static GString* get_filename(gchar *name_default, gchar *name_suffix_default)
{
	const gchar *directory;
	GString *filename;
	guint base_len, i;
	struct stat statbuf;
	/* 定义保存文件夹 */
	directory = SAVE_FOLDER_DEFAULT;
	if(directory == NULL)
	{
	directory = g_get_tmp_dir();
	}
	/* 创建唯一的文件名 */
	filename = g_string_new(g_build_filename(directory, name_default, NULL));
	base_len = filename->len;
	g_string_append(filename, name_suffix_default);
	for(i = 1; !stat(filename->str, &statbuf); ++i)
	{
	g_string_truncate(filename, base_len);
	g_string_append_printf(filename, "%d%s", i, name_suffix_default);
	}
	return filename;
}

/* 本回调函数在用户请求一张照片后将注册到图片库上 */
static gboolean buffer_probe_callback(
GstElement *image_sink,
GstBuffer *buffer, GstPad *pad, AppData *appdata)
{
	/* 原始 RGB 数据 */
	unsigned char *data_photo = (unsigned char *) GST_BUFFER_DATA(buffer);
	/* 创建一个 JPEG 数据并检查其状态 */
	if(!create_jpeg(data_photo))
		printf("take-photo-failed");
	/* 断开处理函数的连接,这样不会拍摄更多的照片 */
	g_signal_handler_disconnect(G_OBJECT(image_sink), appdata->buffer_cb_id);
	return TRUE;
}
/* 当用户点击<Take photo>按钮时此回调函数将被调用 */
static void take_photo(GtkWidget *widget, AppData *appdata)
{
	GstElement *image_sink;
	/* 从管道中获得图像数据 */
	image_sink = gst_bin_get_by_name(GST_BIN(appdata->pipeline), "image_sink");
	/* 将图像的"handoff"信号连接到回调函数上 */
	appdata->buffer_cb_id = g_signal_connect(G_OBJECT(image_sink), "handoff",
	G_CALLBACK(buffer_probe_callback), appdata);
}
/* 当用户点击< Take video >按钮时此回调函数将被调用 */
static void take_video(GtkWidget *widget, AppData *appdata)
{
	if(!create_avi(appdata))
	{
		printf("take_vido_failed");
	}
}

/* 只要管道消息总线有消息,此回调函数将被调用 */
static void bus_callback(GstBus *bus, GstMessage *message, AppData *appdata)
{
	gchar *message_str;
	const gchar *message_name;
	GError *error;
	/* 向控制台报告错误 */
	if(GST_MESSAGE_TYPE(message) == GST_MESSAGE_ERROR)
	{
		gst_message_parse_error(message, &error, &message_str);
		g_error("GST error: %s\n", message_str);
		g_free(error);
		g_free(message_str);
	}
	/* 向控制台报告警告信息 */
	if(GST_MESSAGE_TYPE(message) == GST_MESSAGE_WARNING)
	{
		gst_message_parse_warning(message, &error, &message_str);
		g_warning("GST warning: %s\n", message_str);
		g_free(error);
		g_free(message_str);
	}
}
/* 当 screen-widget 显示时本回调函数将被调用 */
static gboolean expose_cb(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
	gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(data),
	GDK_WINDOW_XWINDOW(widget->window));
	return FALSE;
}
/* 在退出时销毁(Destroy)管道 */
static void destroy_pipeline(GtkWidget *widget, AppData *appdata)
{
	/* 释放管道,这也将释放对那些加到此管道上的模块的引用 */
	gst_element_set_state(appdata->pipeline, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(appdata->pipeline));
}
/* 硬件键的回调函数 */
gboolean key_press_cb(GtkWidget * widget, GdkEventKey * event, AppData *appdata)
{
	switch (event->keyval) {
		case GDK_F6:
		take_photo(widget, appdata) ;
		return TRUE;
		case GDK_F7:
		take_video(widget, appdata) ;
		return TRUE;
		case GDK_F8:
		gtk_main_quit() ;
		return TRUE;
	}
	return FALSE;
}
/* 初始化 Gstreamer 管道 */
static gboolean initialize_pipeline(AppData *appdata, int *argc, char ***argv)
{
	GstElement *pipeline, *camera_src, *screen_sink, *image_sink, *video_sink;
	GstElement *screen_queue, *image_queue, *video_queue,
	*video_jpegenc, *video_avimux, *video_filesink;
	GstElement *csp_filter, *image_filter, *tee;
	GstCaps *caps;
	GstBus *bus;
	/* 初始化 Gstreamer */
	gst_init(argc, argv);
	/* 创建管道,并将回调函数附加到它的消息总线上 */
	pipeline = gst_pipeline_new("test-camera");
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, (GstBusFunc)bus_callback, appdata);
	gst_object_unref(GST_OBJECT(bus));
	/* 将管道保存到 AppData 结构中 */
	appdata->pipeline = pipeline;
	/* 来自 Video4Linux 驱动的视频流 */
	camera_src = gst_element_factory_make(VIDEO_SRC, "camera_src");
	/* 需要色彩空间(Colorspace)滤波器来确保接收器理解来自摄像头的流 */
	csp_filter = gst_element_factory_make("ffmpegcolorspace", "csp_filter");
	/* Tee 将流拷贝到多个输出端 s */
	tee = gst_element_factory_make("tee", "tee");
	/* 为流创建新线程队列 */
	screen_queue = gst_element_factory_make("queue", "screen_queue");
	/* 接收器将图像显示在屏幕上 */
	screen_sink = gst_element_factory_make(VIDEO_SINK, "screen_sink");
	/* 为拍摄的图像流创建独立线程 */
	image_queue = gst_element_factory_make("queue", "image_queue");
	/* 滤波器将流转换为 gdkpixbuf 库能使用的格式 */
	image_filter = gst_element_factory_make("ffmpegcolorspace", "image_filter");
	/* 图像流的虚拟接收器,转到 bitheaven */
	image_sink = gst_element_factory_make("fakesink", "image_sink");
	/* 检查元素是否正确地初始化 */
	if(!(pipeline && camera_src && screen_sink && csp_filter && screen_queue
	&& image_queue && image_filter && image_sink ))
	{
	g_critical("Couldn't create pipeline elements");
	return FALSE;
	}
	/* 将图像接收器设置为在扔掉其缓存器之前发出 handoff 信号 */
	g_object_set(G_OBJECT(image_sink), "signal-handoffs", TRUE, NULL);
	/* 将元素加到管道中,此步骤在连接之前做 */
	gst_bin_add_many(GST_BIN(pipeline), camera_src, csp_filter,
	tee, screen_queue, screen_sink, image_queue,
	image_filter, image_sink, NULL);
	/* 说明想从摄像头获得哪种视频 */
	caps = gst_caps_new_simple("video/x-raw-rgb",
	"width", G_TYPE_INT, 640,
	"height", G_TYPE_INT, 480,
	NULL);
	/* 连接摄像头源和色彩空间(colorspace)滤波器 */
	if(!gst_element_link_filtered(camera_src, csp_filter, caps))
	{
	return FALSE;
	}
	gst_caps_unref(caps);
	/* 连接色彩空间滤波器-> Tee ->屏幕队列 -> 屏幕接收器 */
	if(!gst_element_link_many(csp_filter, tee, screen_queue, screen_sink, NULL))
	{
	return FALSE;
	}


	/* gdkpixbuf 要求每次采用 8 位,这样每像素为 24 位 */
	caps = gst_caps_new_simple("video/x-raw-rgb",
	"width", G_TYPE_INT, 640,
	"height", G_TYPE_INT, 480,
	"bpp", G_TYPE_INT, 24,
	"depth", G_TYPE_INT, 24,
	"framerate", GST_TYPE_FRACTION, 15, 1,
	NULL);
	/* 连接管道的图像分支,在此之后管道准备就绪 */
	if(!gst_element_link_many(tee, image_queue, image_filter, NULL)) return FALSE;
	if(!gst_element_link_filtered(image_filter, image_sink, caps)) return FALSE;
	gst_caps_unref(caps);
	/* 只要屏幕显示出来,窗口 IP 将传给接收器 */
	g_signal_connect(appdata->screen, "expose-event", G_CALLBACK(expose_cb), screen_sink);
	/* 将管道状态设置为 GST_STATE_PLAYING */
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	return TRUE;
}



int main(int argc, char **argv)
{
	AppData appdata;
	GtkWidget *window, *button, *hbox, *vbox_button, *button2, *vbox;
	/* 初始化线程环境 */
	g_thread_init(NULL);
	/* 初始化 GTK+ */
	gtk_init(&argc, &argv);
	/* 将初始记录状态设为 to FALSE */
	appdata.record_state = FALSE;
	/* 创建主窗口 */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "camrecorder");
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	vbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);
	vbox_button = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox_button, FALSE, FALSE, 0);
	appdata.screen = gtk_drawing_area_new();
	gtk_widget_set_size_request(appdata.screen, 500, 380);
	gtk_box_pack_start(GTK_BOX(vbox), appdata.screen, FALSE, FALSE, 0);
	button = gtk_button_new_with_label("Take photo");
	gtk_widget_set_size_request(button, 120, 60);
	gtk_box_pack_start(GTK_BOX(vbox_button), button, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(button), "clicked",
	G_CALLBACK(take_photo), &appdata);
	appdata.button = gtk_button_new_with_label("start Recording");
	gtk_widget_set_size_request(appdata.button, 120, 60);
	gtk_box_pack_start(GTK_BOX(vbox_button), appdata.button, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(appdata.button), "clicked",
	G_CALLBACK(take_video), &appdata);
	/* 将硬件按钮侦听器加到应用上 */
	g_signal_connect(G_OBJECT(window),
	"key_press_event", G_CALLBACK(key_press_cb), &appdata);
	gtk_container_add(GTK_CONTAINER(window), hbox);
	/* 初始化 GTK 管道 */
	if(!initialize_pipeline(&appdata, &argc, &argv))
	return 0;
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy_pipeline), &appdata);
	/* 开始主应用 */
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
	return 0;
}

static gboolean create_jpeg(unsigned char *data)
{
	GdkPixbuf *pixbuf = NULL;
	GError *error = NULL;
	guint height, width, bpp;
	GString *filename;
	/* 定义 jpeg 大小 */
	width = 640; height = 480; bpp = 24;
	filename = get_filename(PHOTO_NAME_DEFAULT, PHOTO_NAME_SUFFIX_DEFAULT);
	/* 从数据中创建 pixbuf 对象 */
	pixbuf = gdk_pixbuf_new_from_data(data,
	GDK_COLORSPACE_RGB, /* RGB-colorspace */
	FALSE, /* No alpha-channel */
	bpp/3, /* Bits per RGB-component */
	width, height, /* Dimensions */
	3*width, /* Number of bytes between lines (ie stride) */
	NULL, NULL); /* Callbacks */
	/* 将 pixbuf 的内容保存到 jpeg 文件中,并检查错误 */
	if(!gdk_pixbuf_save(pixbuf, filename->str, "jpeg", &error, NULL))
	{
	g_warning("%s\n", error->message);
	g_error_free(error);
	gdk_pixbuf_unref(pixbuf);
	g_string_free(filename, TRUE);
	return FALSE;
	}
	printf("Saved to %s\n",filename->str ) ;
	/* 释放分配的资源,返回 TRUE,表示操作成功 */
	g_string_free(filename, TRUE);
	gdk_pixbuf_unref(pixbuf);
	return TRUE;
}

camera_src -> csp_filter -> tee -> screen_queue -> screen_sink
						     |
						    tee -> video_filter -> video_queue -> video_jpegenc -> video_avimux -> video_filesink
						    
static gboolean create_avi(AppData *appdata)
{
	GstElement *bin, *pipeline, *video_queue, *video_jpegenc, *video_avimux, *video_filesink, *video_filter;
	GString *filename;
	GstElement *tee;
	pipeline = appdata->pipeline;
	/* 从管道中获得图像接收器元素 */
	tee = gst_bin_get_by_name(GST_BIN(pipeline), "tee");
	bin = gst_bin_new ("my_bin");

	/* 视频 */
	video_queue = gst_element_factory_make("queue", "video_queue");
	video_jpegenc = gst_element_factory_make("jpegenc", "video_jpegenc");
	video_avimux = gst_element_factory_make("avimux", "video_avimux");
	video_filesink = gst_element_factory_make("filesink", "video_filesink");
	video_filter = gst_element_factory_make("ffmpegcolorspace", "video_filter");
	filename = get_filename(VIDEO_NAME_DEFAULT, VIDEO_NAME_SUFFIX_DEFAULT);
	g_object_set(G_OBJECT(video_filesink), "location", filename->str, NULL);
	gst_bin_add_many (GST_BIN (bin), video_queue, video_jpegenc, video_avimux,
	video_filesink, video_filter, NULL);
	if(!gst_element_link_many(video_filter, video_queue, video_jpegenc, video_avimux,
	video_filesink, NULL)) ;

	if(appdata->record_state == FALSE)
	{
		gst_element_set_state(pipeline, GST_STATE_PAUSED);
		gst_bin_add_many(GST_BIN(pipeline), bin, NULL);
		if(!gst_element_link_many(tee, video_filter, NULL)) ;
		gst_element_set_state(pipeline, GST_STATE_PLAYING);
		gtk_button_set_label(GTK_BUTTON(appdata->button), "stop recording");
		appdata->record_state = TRUE;
	}
	else
	{
		gst_element_set_state(pipeline, GST_STATE_PAUSED);
		bin = gst_bin_get_by_name(GST_BIN(pipeline), "my_bin");
		gst_bin_remove_many(GST_BIN(pipeline), bin, NULL);
		gst_element_set_state(bin, GST_STATE_NULL);
		gst_element_set_state(pipeline, GST_STATE_PLAYING);
		gtk_button_set_label(GTK_BUTTON(appdata->button), "start recording");
		printf("Saved to %s\n",filename->str ) ;
		appdata->record_state = FALSE;
	}
	return TRUE;
}