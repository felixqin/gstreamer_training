

#include <stdio.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include "appsrc_context.h"


typedef struct
{
    gboolean white;
    GstClockTime timestamp;
} AppSrcContext;


extern "C" void* create_context()
{
    AppSrcContext* ctx = (AppSrcContext*)malloc(sizeof(AppSrcContext));
    ctx->white = FALSE;
    ctx->timestamp = 0;
    return ctx;
}

extern "C" void destroy_context(void* data)
{
    AppSrcContext* ctx = (AppSrcContext*)data;
    free(ctx);
}

/* called when we need to give data to appsrc */
extern "C" void need_data (GstElement * appsrc, guint unused, void* data)
{
    GstBuffer *buffer;
    guint size;
    GstFlowReturn ret;
    AppSrcContext* ctx = (AppSrcContext*)data;

    auto clock = gst_element_get_clock(appsrc);
    auto abs_time = (clock) ? gst_clock_get_time(clock) : 0;
    auto base_time = gst_element_get_base_time(appsrc);
    auto running_time = abs_time - base_time;
    if (clock) g_object_unref(clock);
    printf("need_data() ctx(%p) clock(%p) abs_time(%llu) base_time(%llu)\n", ctx, clock, abs_time, base_time);

    size = 385 * 288 * 2;

    buffer = gst_buffer_new_allocate (NULL, size, NULL);

    /* this makes the image black/white */
    gst_buffer_memset (buffer, 0, ctx->white ? 0xf0 : 0x0f, size);

    ctx->white = !ctx->white;

    /* increment the timestamp every 1/2 second */
    GST_BUFFER_PTS (buffer) = ctx->timestamp;
    GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 2);
    //if (ctx->timestamp != 0) { GST_BUFFER_PTS(buffer) = -1; }
    GST_BUFFER_PTS(buffer) = running_time;
    ctx->timestamp += GST_BUFFER_DURATION (buffer);
    printf("need data pts(%llu) duration(%llu)\n", GST_BUFFER_PTS(buffer), GST_BUFFER_DURATION(buffer));

    g_signal_emit_by_name (appsrc, "push-buffer", buffer, &ret);
}

extern "C" void enough_data (GstElement*, guint, void* ctx)
{
    printf("enough_data() ctx(%p)\n", ctx);
}
