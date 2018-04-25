

#include <stdlib.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
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
        GST_MEMORY_FLAG_READONLY,
        frame.data.get(), frame.size, 0, frame.size,
        new std::shared_ptr<uint8_t>(frame.data), default_delete<std::shared_ptr<uint8_t>>
    );
}

////////////////////////////////////////////////////////////////////////////////

struct AppSrcContext
{
    GstElement* appsrc;
    IStreamSourcePtr stream;
};

extern "C" void* create_context()
{
    auto ctx = new AppSrcContext();
    ctx->appsrc = nullptr;
    return ctx;
}

extern "C" void destroy_context(void* data)
{
    auto ctx = (AppSrcContext*)data;
    delete ctx;
}

static void on_stream(AppSrcContext* ctx, IStreamSource::Frame const& frame)
{
    auto buffer = create_buffer(frame);

    /* increment the timestamp every 1/2 second */
    GST_BUFFER_PTS (buffer) = frame.pts * 1000; // ns
    GST_BUFFER_DURATION (buffer) = frame.duration * 1000;

    GstFlowReturn ret = GST_FLOW_OK;
    g_signal_emit_by_name (ctx->appsrc, "push-buffer", buffer, &ret);
    LOGI("push buffer! ret(%d)\n", ret);
}

/* called when we need to give data to appsrc */
extern "C" void start_feed(GstElement* appsrc, guint unused, void* data)
{
    LOGI("start_feed\n");
    auto ctx = (AppSrcContext*)data;
    if (!ctx->stream)
    {
        ctx->appsrc = appsrc;
        ctx->stream = getRawStreamSource();
        ctx->stream->start([ctx](IStreamSource::Frame const& frame) {
            on_stream(ctx, frame);
        });
    }
}

extern "C" void stop_feed(GstElement* appsrc, guint unused, void* data)
{
    LOGI("stop_feed\n");
    auto ctx = (AppSrcContext*)data;
    if (ctx->stream)
    {
        ctx->stream->stop();
        ctx->stream.reset();
    }
}
