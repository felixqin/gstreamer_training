
#include <windows.h>
#include <unistd.h>
#include <thread>
#include "StreamSource.h"


#define LOGI printf


class CRawSource : public IStreamSource
{
public:
    static IStreamSourcePtr instance()
    {
        static std::shared_ptr<CRawSource> inst = std::shared_ptr<CRawSource>(new CRawSource());
        return inst;
    }

    ~CRawSource() final
    {
        stop();
    }

    bool start(StreamCallback callback) final
    {
        LOGI("CRawThread::start()\n");
        mCallback = callback;
        mExitFlag = false;
        mThread = std::make_unique<std::thread>([this](){
            threadProc();
        });
        return true;
    }

    bool stop() final
    {
        LOGI("CRawThread::stop()\n");
        mExitFlag = true;
        mThread->join();
        mThread.reset();
        return true;
    }

private:
    CRawSource()
    {
        int w = 384, h = 288;
        auto bytes = w * h * 2;
        mFrame = Frame{
            std::shared_ptr<uint8_t>((uint8_t*)malloc((size_t)bytes), free),
            (size_t)bytes,
            0
        };
    }

    void threadProc()
    {
        LOGI("CRawThread::threadProc() begin\n");

        uint64_t pts = 0;
        while (!mExitFlag)
        {
            LOGI("CRawThread::threadProc() loop! color(%02x)\n", mColor);

            //auto now = GetTickCount();
            memset(mFrame.data.get(), mColor, mFrame.size);
            mFrame.pts = pts;
            mFrame.duration = 500;
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

            mCallback(mFrame);

            pts += 500;
            usleep(500*1000);
        }

        LOGI("CRawThread::threadProc() end\n");
    }

private:
    std::unique_ptr<std::thread> mThread;
    volatile bool mExitFlag = false;
    StreamCallback mCallback;
    Frame mFrame;
    uint8_t mColor = 0;
};

IStreamSourcePtr getRawStreamSource()
{
    return CRawSource::instance();
}
