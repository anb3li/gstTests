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
        "input-selector name=selector ! videoconvert ! autovideosink 
         videotestsrc pattern = 0 ! selector.sink_1
         videotestsrc pattern = 1 ! selector.sink_2",
        NULL);

    GstElement *selector = gst_bin_get_by_name(GST_BIN(pipeline), "selector");

    GstPad vid2sink = gst_element_get_static_pad(selector, "sink_2");

    // Start playing the pipeline
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Sleep for 5 seconds to display the first source
    g_print("Displaying first source for 5 seconds...\n");
    g_usleep(5 * G_USEC_PER_SEC);

    // Switch to the second source (videotestsrc pattern=1)
    g_print("Switching to second source...\n");
    g_object_set(selector, "active-pad", vid2sink, NULL);

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
