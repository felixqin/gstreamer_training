

#include <stdlib.h>
#include <mutex>
#include <deque>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include "StreamSource.h"
#include "appsrc_context.h"


#define LOGI(fmt, arg...) printf(fmt, ##arg)


////////////////////////////////////////////////////////////////////////////////


IStreamSourcePtr getRawStreamSource();

inline IStreamSourcePtr getStreamSource()
{
    return getRawStreamSource();
}

////////////////////////////////////////////////////////////////////////////////

template <class T>
inline void default_delete(void* ptr)
{
    auto p = static_cast<T*>(ptr);
    delete p;
}

inline GstBuffer* create_buffer(IStreamSource::Frame const& frame)
{
    return gst_buffer_new_wrapped_full(
            GstMemoryFlags(0), //GST_MEMORY_FLAG_READONLY,
        frame.data.get(), frame.size, 0, frame.size,
        new std::shared_ptr<uint8_t>(frame.data), default_delete<std::shared_ptr<uint8_t>>
    );
}

////////////////////////////////////////////////////////////////////////////////

struct AppSrcContext
{
    GstAppSrc* appsrc;
    guint sourceid;
    IStreamSourcePtr stream;

    std::mutex mtx;
    std::deque<IStreamSource::Frame> frame_queue;
};

extern "C" void* create_context(GstElement* appsrc)
{
    auto ctx = new AppSrcContext();
    ctx->appsrc = (GstAppSrc*)appsrc;
    ctx->sourceid = 0;
    return ctx;
}

extern "C" void destroy_context(void* data)
{
    auto ctx = (AppSrcContext*)data;
    delete ctx;
}

static void on_stream(AppSrcContext* ctx, IStreamSource::Frame const& frame)
{
    std::lock_guard<std::mutex> lock(ctx->mtx);
    ctx->frame_queue.push_back(frame);
}

extern "C" gboolean on_push_data(gpointer data)
{
    auto ctx = (AppSrcContext*)data;

    /*
    IStreamSource::Frame frame;
    {
        std::lock_guard<std::mutex> lock(ctx->mtx);
        if (ctx->frame_queue.empty())
        {
            //LOGI("frame queue is empty!\n");
            return TRUE;
        }

        frame = ctx->frame_queue.front();
        ctx->frame_queue.pop_front();
    }
     */

    //auto buffer = create_buffer(frame);
    auto size = 385 * 288 * 2;
    auto buffer = gst_buffer_new_allocate (NULL, size, NULL);
    gst_buffer_memset (buffer, 0, 0xf0, size);

    /* increment the timestamp every 1/2 second */
    static uint64_t pts = 0;
    GST_BUFFER_PTS (buffer) = pts*1000*1000; //frame.pts * 1000000; // ns
    GST_BUFFER_DURATION (buffer) = 500*1000*1000;//frame.duration * 1000000;
    pts += 500;

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
extern "C" void start_feed(GstElement* appsrc, guint length, void* data)
{
    LOGI("start_feed appsrc(%p) length(%d) data(%p)\n", appsrc, length, data);
    auto ctx = (AppSrcContext*)data;
    if (ctx->sourceid == 0)
    {
        ctx->sourceid = g_idle_add((GSourceFunc)on_push_data, ctx);
        ctx->stream = getRawStreamSource();
        //ctx->stream->start([ctx](IStreamSource::Frame const& frame) {
            //on_stream(ctx, frame);
        //});
    }
}

extern "C" void stop_feed(GstElement* appsrc, void* data)
{
    LOGI("stop_feed appsrc(%p) data(%p)\n", appsrc, data);
    auto ctx = (AppSrcContext*)data;
    if (ctx->sourceid != 0)
    {
        g_source_remove(ctx->sourceid);
        ctx->sourceid = 0;
        //ctx->stream->stop();
        ctx->stream.reset();
    }
}
