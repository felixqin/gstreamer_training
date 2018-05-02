
#include "frame_source.h"
#include <windows.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <deque>


#define LOGI printf


class CFrameSource
{
public:
    CFrameSource()
    {
    }

    ~CFrameSource()
    {
        stop();
    }

    bool start()
    {
        LOGI("CRawThread::start()\n");
        mExitFlag = false;
        mThread = std::make_unique<std::thread>([this](){
            threadProc();
        });
        return true;
    }

    bool stop()
    {
        LOGI("CRawThread::stop()\n");
        mExitFlag = true;
        mThread->join();
        mThread.reset();
        return true;
    }

    GstBuffer* getFrame()
    {
        std::lock_guard<std::mutex> guard(mMutex);
        if (!mFrameQueue.empty())
        {
            auto frame = mFrameQueue.front();
            mFrameQueue.pop_front();
            return frame;
        }

        return nullptr;
    }

private:
    void threadProc()
    {
        LOGI("CRawThread::threadProc() begin\n");

        uint64_t pts = 0;
        while (!mExitFlag)
        {
            LOGI("CRawThread::threadProc() loop! color(%02x)\n", mColor);

            int w = 384, h = 288;
            auto bytes = w * h * 2;
            auto buffer = gst_buffer_new_allocate (NULL, bytes, NULL);
            gst_buffer_memset (buffer, 0, mColor, bytes);

            /* increment the timestamp every 1/2 second */
            GST_BUFFER_PTS (buffer) = pts*1000*1000; //frame.pts * 1000000; // ns
            GST_BUFFER_DURATION (buffer) = 500*1000*1000;//frame.duration * 1000000;

            if (mColor == 0)
            {
                mColor = 0xf;
            }
            else if (mColor == 0xf)
            {
                mColor = 0xf0;
            }
            else
            {
                mColor = 0;
            }

            {
                std::lock_guard<std::mutex> guard(mMutex);
                mFrameQueue.push_back(buffer);
            }

            pts += 500;
            usleep(500*1000);
        }

        LOGI("CRawThread::threadProc() end\n");
    }

private:
    std::unique_ptr<std::thread> mThread;
    volatile bool mExitFlag = false;
    uint8_t mColor = 0;

    std::mutex mMutex;
    std::deque<GstBuffer*> mFrameQueue;
};


extern "C" void* create_frame_source()
{
    return new CFrameSource();
}

extern "C" void destroy_frame_source(void* handle)
{
    auto source = (CFrameSource*)handle;
    delete source;
}

extern "C" void frame_source_start(void* handle)
{
    auto source = (CFrameSource*)handle;
    source->start();
}

extern "C" void frame_source_stop(void* handle)
{
    auto source = (CFrameSource*)handle;
    source->stop();
}

extern "C" GstBuffer* frame_source_get_frame(void* handle)
{
    auto source = (CFrameSource*)handle;
    return source->getFrame();
}

