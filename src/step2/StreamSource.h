#pragma once

#include <memory>
#include <functional>


class IStreamSource
{
public:
    struct Frame
    {
        std::shared_ptr<uint8_t> data;
        size_t size;
        uint64_t pts;
        uint64_t duration;
    };

    typedef std::function<void(Frame const&)> StreamCallback;

public:
    virtual ~IStreamSource() {}

    virtual bool start(StreamCallback) = 0;

    virtual bool stop() = 0;
};

typedef std::shared_ptr<IStreamSource> IStreamSourcePtr;

