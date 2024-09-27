#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create pipeline using gst_parse_launch
    pipeline = gst_parse_launch(
        "\
   videotestsrc pattern=1 ! \
   video/x-raw,format=AYUV,framerate=\(fraction\)10/1,width=100,height=100 ! \
   videobox border-alpha=0 top=-70 bottom=-70 right=-220 ! \
   compositor name=comp sink_0::alpha=0.7 sink_1::alpha=0.5 ! \
   videoconvert ! xvimagesink \
   videotestsrc ! \
   video/x-raw,format=AYUV,framerate=\(fraction\)5/1,width=320,height=240 ! comp.",
        NULL);

    // Start playing the pipeline
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Wait until error or EOS (End of Stream)
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // Free resources
    if (msg != NULL)
        gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
