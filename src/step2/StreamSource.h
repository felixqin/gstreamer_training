#pragma once

#include <memory>
#include <functional>


class IStreamSource
{
public:
    typedef std::shared_ptr<uint8_t> Frame;
    typedef std::function<void(Frame, uint64_t pts)> StreamCallback;

public:
    virtual ~IStreamSource() = 0;

    virtual bool start() = 0;

    virtual bool stop() = 0;
};

typedef std::shared_ptr<IStreamSource> IStreamSourcePtr;

