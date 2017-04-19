//gstreamer pipeline:
//
//gst-launch-1.0 -v v4l2src ! 'video/x-raw, width=640, height=480, framerate=30/1' ! queue ! videoconvert ! omxh264enc ! h264parse ! flvmux ! rtmpsink location='rtmp://{MY_IP}/rtmp/live'
void threadgst(){

    App * app = &s_app; 
    GstCaps *srccap;
    GstCaps * filtercap;
    GstFlowReturn ret;
    GstBus *bus;
    GstElement *pipeline;

    gst_init (NULL,NULL);

    loop = g_main_loop_new (NULL, TRUE);

    //creazione della pipeline:
    pipeline = gst_pipeline_new ("gstreamer-encoder");
    if( ! pipeline ) {
        g_print("Error creating Pipeline, exiting...");
    }

    //creazione elemento appsrc:
    app-> videosrc = gst_element_factory_make ("appsrc", "videosrc");
    if( !  app->videosrc ) {
            g_print( "Error creating source element, exiting...");
    }

    //creazione elemento queue:
    app-> queue = gst_element_factory_make ("queue", "queue");
    if( !  app->queue ) {
            g_print( "Error creating queue element, exiting...");
    }

    app->videocoverter = gst_element_factory_make ("videoconvert", "videocoverter");
    if( ! app->videocoverter ) {
            g_print( "Error creating videocoverter, exiting...");
    }

    //creazione elemento filter:
    app->filter = gst_element_factory_make ("capsfilter", "filter");
    if( ! app->filter ) {
            g_print( "Error creating filter, exiting...");
    }

    app->h264enc = gst_element_factory_make ("omxh264enc", "h264enc");
    if( ! app->h264enc ) {
            g_print( "Error creating omxh264enc, exiting...");
    }

 app->h264parse = gst_element_factory_make ("h264parse", "h264parse");
    if( ! app->h264parse ) {
            g_print( "Error creating h264parse, exiting...");
    }
    app->flvmux = gst_element_factory_make ("flvmux", "flvmux");
    if( ! app->flvmux ) {
            g_print( "Error creating flvmux, exiting...");
    }
    app->rtmpsink = gst_element_factory_make ("rtmpsink", "rtmpsink");
    if( ! app->rtmpsink ) {
            g_print( "Error rtmpsink flvmux, exiting...");
    }



    g_print ("Elements are created\n");
    g_object_set (G_OBJECT (app->rtmpsink), "location" , "rtmp://192.168.3.107/rtmp/live live=1" ,  NULL);
    


    g_print ("end of settings\n");

    srccap = gst_caps_new_simple("video/x-raw",
            "format", G_TYPE_STRING, "RGB",
            "width", G_TYPE_INT, 640,
            "height", G_TYPE_INT, 480,
            //"width", G_TYPE_INT, 320,
            //"height", G_TYPE_INT, 240,
            "framerate", GST_TYPE_FRACTION, 30, 1,
            //"pixel-aspect-ratio", GST_TYPE_FRACTION, 1, 1,
        NULL);

    filtercap = gst_caps_new_simple("video/x-raw",
            "format", G_TYPE_STRING, "I420",
            "width", G_TYPE_INT, 640,
            "height", G_TYPE_INT, 480,
            //"width", G_TYPE_INT, 320,
            //"height", G_TYPE_INT, 240,
            "framerate", GST_TYPE_FRACTION, 30, 1,
        NULL);

    gst_app_src_set_caps(GST_APP_SRC( app->videosrc), srccap);
    g_object_set (G_OBJECT (app->filter), "caps", filtercap, NULL);


    bus = gst_pipeline_get_bus (GST_PIPELINE ( pipeline));

    g_assert(bus);
    gst_bus_add_watch ( bus, (GstBusFunc) bus_call, app);

 gst_bin_add_many (GST_BIN ( pipeline), app-> videosrc, app->queue, app->videocoverter,app->filter, app->h264enc,  app->h264parse, app->flvmux, app->rtmpsink, NULL);

    g_print ("Added all the Elements into the pipeline\n");

    int ok = false;
    ok = gst_element_link_many ( app-> videosrc, app->queue, app->videocoverter, app->filter,app->h264enc,  app->h264parse, app->flvmux, app->rtmpsink, NULL);


    if(ok)g_print ("Linked all the Elements together\n");
    else g_print("*** Linking error ***\n");

    g_assert(app->videosrc);
    g_assert(GST_IS_APP_SRC(app->videosrc));

    g_signal_connect (app->videosrc, "need-data", G_CALLBACK (start_feed), app);
    g_signal_connect (app->videosrc, "enough-data", G_CALLBACK (stop_feed),app);


    g_print ("Playing the video\n");
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    g_print ("Running...\n");
        g_main_loop_run ( loop);

    g_print ("Returned, stopping playback\n");
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref ( bus);
    g_main_loop_unref (loop);
    g_print ("Deleting pipeline\n");


}

