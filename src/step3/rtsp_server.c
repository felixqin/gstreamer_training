/* GStreamer
 * Copyright (C) 2008 Wim Taymans <wim.taymans at gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <stdlib.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <gst/app/gstappsrc.h>
#include "frame_source.h"


#define LOGI(fmt, arg...) g_print(fmt, ##arg)


typedef struct
{
    GstAppSrc* appsrc;
    guint sourceid;
    void* stream;
} MediaContext;


static void* create_context(GstElement* appsrc)
{
    MediaContext* ctx = malloc(sizeof(MediaContext));
    ctx->appsrc = (GstAppSrc*)appsrc;
    ctx->sourceid = 0;
    ctx->stream = NULL;
    return ctx;
}

static void destroy_context(MediaContext* ctx)
{
    if (ctx->sourceid != 0)
    {
        g_source_remove(ctx->sourceid);
        ctx->sourceid = 0;
    }

    if (ctx->stream)
    {
        frame_source_stop(ctx->stream);
        destroy_frame_source(ctx->stream);
    }

    free(ctx);
}

static gboolean on_push_data(MediaContext* ctx)
{
    GstBuffer* buffer = frame_source_get_frame(ctx->stream);
    if (!buffer)
    {
        //LOGI("frame queue is empty!\n");
        return TRUE;
    }

    //LOGI("to push buffer!\n");
    //g_signal_emit_by_name(ctx->appsrc, "push-buffer", buffer, &ret);
    GstFlowReturn ret = gst_app_src_push_buffer(ctx->appsrc, buffer);
    LOGI("push buffer! ret(%d)\n", ret);

    if (ret != GST_FLOW_OK) {
        /* We got some error, stop sending data */
        return FALSE;
    }

    return TRUE;
}

/* called when we need to give data to appsrc */
static void start_feed(GstElement* appsrc, guint length, MediaContext* ctx)
{
    LOGI("start_feed appsrc(%p) length(%d) ctx(%p)\n", appsrc, length, ctx);
    if (!ctx->stream)
    {
        ctx->stream = create_frame_source();
        frame_source_start(ctx->stream);
    }

    if (ctx->sourceid == 0)
    {
        ctx->sourceid = g_idle_add((GSourceFunc)on_push_data, ctx);
        LOGI("ctx sourceid(%d)\n", ctx->sourceid);
    }
}

static void stop_feed(GstElement* appsrc, MediaContext* ctx)
{
    LOGI("stop_feed appsrc(%p) ctx(%p)\n", appsrc, ctx);
    if (ctx->sourceid != 0)
    {
        g_source_remove(ctx->sourceid);
        ctx->sourceid = 0;
    }
}

/* called when a new media pipeline is constructed. We can query the
 * pipeline and configure our appsrc */
static void media_configure (GstRTSPMediaFactory * factory, GstRTSPMedia * media,
                 gpointer data)
{
    GstElement *element, *appsrc;
    void *ctx;

    /* get the element used for providing the streams of the media */
    element = gst_rtsp_media_get_element (media);

    /* get our appsrc, we named it 'mysrc' with the name property */
    appsrc = gst_bin_get_by_name_recurse_up (GST_BIN (element), "mysrc");

    /* this instructs appsrc that we will be dealing with timed buffer */
    gst_util_set_object_arg (G_OBJECT (appsrc), "format", "time");
    //g_object_set (G_OBJECT (appsrc), "format", GST_FORMAT_TIME, NULL);
    /* configure the caps of the video */
    g_object_set (G_OBJECT (appsrc), "caps",
                  gst_caps_new_simple ("video/x-raw",
                                       "format", G_TYPE_STRING, "RGB16",
                                       "width", G_TYPE_INT, 384,
                                       "height", G_TYPE_INT, 288,
                                       /*"framerate", GST_TYPE_FRACTION, 0, 1,*/ NULL), NULL);

    ctx = create_context(appsrc);
    /* make sure ther datais freed when the media is gone */
    g_object_set_data_full (G_OBJECT (media), "my-extra-data", ctx,
                            (GDestroyNotify) destroy_context);

    /* install the callback that will be called when a buffer is needed */
    g_signal_connect (appsrc, "need-data", G_CALLBACK(start_feed), ctx);
    g_signal_connect (appsrc, "enough-data", G_CALLBACK(stop_feed), ctx);

    gst_object_unref (appsrc);
    gst_object_unref (element);
}

int rtsp_server_main (int argc, char *argv[])
{
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;

    gst_init (&argc, &argv);

    loop = g_main_loop_new (NULL, FALSE);

    /* create a server instance */
    server = gst_rtsp_server_new ();

    /* get the mount points for this server, every server has a default object
     * that be used to map uri mount points to media factories */
    mounts = gst_rtsp_server_get_mount_points (server);

    /* make a media factory for a test stream. The default media factory can use
     * gst-launch syntax to create pipelines.
     * any launch line works as long as it contains elements named pay%d. Each
     * element with pay%d names will be a stream */
    factory = gst_rtsp_media_factory_new ();
    gst_rtsp_media_factory_set_launch (factory,
                                       "( appsrc name=mysrc ! videoconvert ! video/x-raw,format=YV12 ! x264enc ! rtph264pay name=pay0 pt=96 )");

    /* notify when our media is ready, This is called whenever someone asks for
     * the media and a new pipeline with our appsrc is created */
    g_signal_connect (factory, "media-configure", (GCallback) media_configure, NULL);

    /* attach the test factory to the /test url */
    gst_rtsp_mount_points_add_factory (mounts, "/test", factory);

    /* don't need the ref to the mounts anymore */
    g_object_unref (mounts);

    /* attach the server to the default maincontext */
    gst_rtsp_server_attach (server, NULL);

    /* start serving */
    g_print ("stream ready at rtsp://127.0.0.1:8554/test\n");
    g_main_loop_run (loop);

    return 0;
}
