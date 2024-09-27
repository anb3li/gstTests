#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *selector, *sink_pad_1, *sink_pad_2;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create pipeline using gst_parse_launch
    pipeline = gst_parse_launch(
        "videotestsrc pattern=0 ! video/x-raw,width=640,height=480 ! inputselector name=selector ! autovideosink "
        "videotestsrc pattern=1 ! video/x-raw,width=640,height=480 ! selector.sink_1", 
        NULL);

    // Get the inputselector element by name
    selector = gst_bin_get_by_name(GST_BIN(pipeline), "selector");

    // Start playing the pipeline
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Retrieve pads for switching
    sink_pad_1 = gst_element_get_static_pad(selector, "sink_1");

    // Sleep for 5 seconds to display the first source
    g_print("Displaying first source for 5 seconds...\n");
    g_usleep(5 * G_USEC_PER_SEC);

    // Switch to the second source (videotestsrc pattern=1)
    g_print("Switching to second source...\n");
    g_object_set(selector, "active-pad", sink_pad_1, NULL);

    // Sleep for another 5 seconds to display the second source
    g_usleep(5 * G_USEC_PER_SEC);

    // Free resources
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(sink_pad_1);
    gst_object_unref(selector);
    gst_object_unref(pipeline);

    return 0;
}
