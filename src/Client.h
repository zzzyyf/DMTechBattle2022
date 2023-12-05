/**
 * 1. 读取文件，按大小均分起始位置，各自顺延起始位置到下一行开始，然后记录下所有的起始位置
 * 2. 解析文件，编码请求（u32请求数 +（u32操作数个数 + 操作数列表））
 * 3. epoll 网络通信（支持 unix domain socket 和普通 tcp socket）
 * 4. 解析响应（u32结果数 + 结果列表）
*/
#pragma once
#include "Common.h"
#include "ringbuffer.h"
#include "FileParser.h"
#include <mutex>

namespace dm {

class Client
{
public:
    u32 mId{0};

    FILE* mInput{nullptr};   // input file handler
    FILE* mOutput{nullptr};  // output file handler

    std::shared_ptr<dm::socket> mSocket{nullptr};

    char* mRequestBuffer{nullptr};
    static constexpr u32 mRequestBufferSize = 16 * 1024 * 1024;
    u32 mBufferedReqSize{0};
    u32 mBufferedReqCount{0};
    u32 mReqCount{0};

    FileStepLineParser  mInputParser;

    std::mutex _mLock;

    Client()
    :
    mRequestBuffer(new char[mRequestBufferSize]),
    mInputParser(mRequestBuffer, mRequestBufferSize)
    {

    }

    ~Client()
    {
        delete[] mRequestBuffer;
    }

    bool setInput(FILE *file, const u64 start_off)
    {
        if (!mInputParser.setFile(file, start_off))
            return false;

        mInput = file;
        return true;
    }

    void run()
    {
        while (mInputParser.parseLine())
        {
            mReqCount++;
        }
    }

};


}   // end of namespace dm;
