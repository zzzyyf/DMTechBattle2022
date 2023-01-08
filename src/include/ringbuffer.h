#pragma once
#include "common.h"

namespace dm {
class RingBuffer
{
    char*   mData{nullptr};
    const u32     mCapacity;

    u32     mSize{0};
    u32     mReadPos{0};
    u32     mWritePos{0};

    u32     mLastNumStart{0};
    char*   mNumberBuffer{nullptr};
    const u32 mMaxNumberLen{21};

public:
    RingBuffer(u32 size) :
    mCapacity{size}
    {
        mData = new char[mCapacity];
        mNumberBuffer = new char[mMaxNumberLen + 1];
        mNumberBuffer[mMaxNumberLen] = '\0';
    }

    ~RingBuffer()
    {
        delete[] mData;
    }

    u32 size() { return mSize; }

    u32 freeSize() { return mCapacity - mSize; }

    u32 singleWriteSize()
    {
        return std::min(mCapacity - mWritePos, mCapacity - mSize);
    }

    u32 read(u32 size, char* dst)
    {
        if (size > mSize || size == 0)
            return 0;
        
        u32 nread = std::min(size, mCapacity - mReadPos);
        memcpy(dst, &mData[mReadPos], nread);
        size -= nread;
        mReadPos += nread;
        if (mReadPos == mCapacity)
            mReadPos = 0;

        if (size > 0)
        {
            memcpy(dst + nread, &mData[mReadPos], size);
            mReadPos += size;
            nread += size;
        }

        return nread;
    }


    u32 read(u32 size, char* dst, u32 &itr)
    {
        if (size > mSize || size == 0)
            return 0;
        
        u32 nread = std::min(size, mCapacity - itr);
        memcpy(dst, &mData[itr], nread);
        size -= nread;
        itr += nread;
        if (itr == mCapacity)
            itr = 0;

        if (size > 0)
        {
            memcpy(dst + nread, &mData[itr], size);
            itr += size;
            nread += size;
        }

        return nread;
    }

    bool hasNext(const u32 itr)
    {
        return itr != mWritePos;
    }


    char next(u32 &itr)
    {
        char c = mData[itr];
        ++itr;
        if (itr == mCapacity)
            itr = 0;
        return c;
    }


    u32 write(u32 size, const char* src)
    {
        if (mCapacity - mSize < size || size == 0)
            return 0;
        
        u32 nwrite = std::min(size, mCapacity - mWritePos);
        memcpy(&mData[mWritePos], src, nwrite);
        size -= nwrite;
        mWritePos += nwrite;
        if (mWritePos == mCapacity)
            mWritePos = 0;

        if (size > 0)
        {
            memcpy(&mData[mWritePos], src + nwrite, size);
            mWritePos += size;
            nwrite += size;
        }

        mSize += nwrite;
        return nwrite;
    }


    u32 singleWrite(u32 size, const char* src)
    {
        u32 max_write = std::min(mCapacity - mWritePos, mCapacity - mSize);
        if (max_write < size || size == 0)
            return 0;
        
        memcpy(&mData[mWritePos], src, size);
        mWritePos += size;
        if (mWritePos == mCapacity)
            mWritePos = 0;
        
        mSize += size;
        return size;
    }

    char* singleWritePos() { return &mData[mWritePos]; }

    void updateForSingleWrite(u32 size)
    {
        mWritePos += size;
        if (mWritePos == mCapacity)
            mWritePos = 0;
        
        mSize += size;
    }

    u64 tryParseNumber(u32 &itr, bool &parsed, bool &completed)
    {
        u64 num = 0;
        while (itr != mWritePos)
        {
            char c = mData[itr];
            if (c == ',')
            {
                char* num_buffer;
                if (itr < mLastNumStart)
                {
                    u64 size1 = mCapacity - mLastNumStart;
                    assert(size1 + itr <= mMaxNumberLen);
                    memcpy(mNumberBuffer, &mData[mLastNumStart], size1);
                    memcpy(&mNumberBuffer[size1], &mData[0], itr);
                    num_buffer = mNumberBuffer;
                }
                else
                {
                    num_buffer = &mData[mLastNumStart];
                }

                num = strtoll(num_buffer, nullptr, 10);
                parsed = true;
                next(itr);
                mLastNumStart = itr;
                return num;
            }
            else if (c == '\n')
            {
                completed = true;
                next(itr);
                mLastNumStart = itr;
                return num;
            }
            else
            {
                next(itr);
            }
        }

        return num;
    }
};

}   // end of namespace dm
