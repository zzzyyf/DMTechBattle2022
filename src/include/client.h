/**
 * 1. 读取文件，按大小均分起始位置，各自顺延起始位置到下一行开始，然后记录下所有的起始位置
 * 2. 解析文件，编码请求（u32请求数 +（u32操作数个数 + 操作数列表））
 * 3. epoll 网络通信（支持 unix domain socket 和普通 tcp socket）
 * 4. 解析响应（u32结果数 + 结果列表）
*/
#pragma once
#include "common.h"
#include "ringbuffer.h"
#include <mutex>

namespace dm {

class Client
{
public:
    u32 mId{0};

    FILE* mInput{nullptr};   // input file handler
    FILE* mOutput{nullptr};  // output file handler

    std::shared_ptr<dm::socket> mSocket{nullptr};

    // this size should at least enough for a whole request, i.e. 21 * 1000
    const u32 mReadBufferSize = 2 * 1024 * 1024;
    char* mReadBuffer;
    u32 mRead{0};
    u32 mParsedOffset{0};

    u64 mTotalRead{0};
    u64 mTotalLen{0};

    char* mRequestBuffer{nullptr};
    const u32 mRequestBufferSize = 16 * 1024 * 1024;
    u32 mBufferedReqSize{0};
    u32 mBufferedReqCount{0};
    u32 mReqCount{0};

    std::mutex _mLock;

    Client()
    {
        mReadBuffer = new char[mReadBufferSize];
        mRequestBuffer = new char[mRequestBufferSize];
    }

    ~Client()
    {
        delete[] mReadBuffer;
        delete[] mRequestBuffer;
    }

    void run()
    {
        u64 rslt;
        while (mTotalRead < mTotalLen)
        {
            // clear parsed input
            mRead -= mParsedOffset;
            moveToHead(mReadBuffer, &mReadBuffer[mParsedOffset], mRead);
            mParsedOffset = 0;

            // read new input into buffer
            u64 size = std::min((u64)(mReadBufferSize - mRead), mTotalLen - mTotalRead);
            rslt = fread(&mReadBuffer[mRead], 1, size, mInput);
            assert(rslt == size);
            mTotalRead += size;
            mRead += size;

            // wait for async send to complete
            mBufferedReqSize = 0;   // debug only
            
            // parse & encode
            u32 idx = mParsedOffset;
            u32 last_num_start = idx;
            u64 num;

            u32 n_num = 0;
            char* cur_req_head = &mRequestBuffer[mBufferedReqSize];
            char* cur_num = &mRequestBuffer[mBufferedReqSize + sizeof(u32)];

            mReqCount += mBufferedReqCount;
            mBufferedReqCount = 0;

            bool req_buffer_full = false;
            while (idx < mRead)
            {
                // try to parse a request
                char c = mReadBuffer[idx];
                if (c == ',' || c == '\r')
                {
                    if ((mRequestBuffer + mRequestBufferSize) < cur_num + sizeof(u64))
                    {
                        req_buffer_full = true;
                        break;
                    }
                    num = strtoll(&mReadBuffer[last_num_start], nullptr, 10);
                    *((u64*)cur_num) = num;

                    cur_num += sizeof(u64);
                    ++n_num;
                    last_num_start = idx + 1;

                    // std::cout << "parsed num #" << n_num << ": " << num << "\n";
                }
                else if (c == '\n')
                {
                    ++mBufferedReqCount;
                    mBufferedReqSize += cur_num - cur_req_head - sizeof(u64);
        
                    // finish current request
                    *((u32*)cur_req_head) = n_num;
                    cur_num -= sizeof(u32);
                    cur_req_head = cur_num - sizeof(u32);

                    last_num_start = idx + 1;
                    mParsedOffset = idx + 1;

                    std::cout << "parsed a request with " << n_num << " numbers.\n";

                    n_num = 0;
                }

                idx++;
            }

            if (mParsedOffset == 0)
            {
                assert(false);
            }

            // when buffer is full, send
            if ((mRequestBuffer + mRequestBufferSize) < cur_num + sizeof(u64))
            {
                /*
                mSocket->async_send(asio::buffer(&mReqCount, sizeof(mReqCount)),
                    [](const asio::error_code &ec, std::size_t byte_transferred){});
                mSocket->async_send(asio::buffer(&mRequestBuffer, mBufferedReqSize),
                    [](const asio::error_code &ec, std::size_t byte_transferred){});
                */
            }
            // and do async_read

        }

        // send all left data in the buffer
        if (mBufferedReqCount > 0)
        {
            /*
            mSocket->async_send(asio::buffer(&mReqCount, sizeof(mReqCount)),
                [](const asio::error_code &ec, std::size_t byte_transferred){});
            mSocket->async_send(asio::buffer(&mRequestBuffer, mBufferedReqSize),
                [](const asio::error_code &ec, std::size_t byte_transferred){});
            */
            mReqCount += mBufferedReqCount;
        }
        // and do async_read
    }
    

    void parseInput()
    {
        
    }

};



}   // end of namespace dm;
