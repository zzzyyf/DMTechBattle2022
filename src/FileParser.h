/**
 * @file FileParser.h
 * @author Zhao Yuanfang (zhaoyuanfang@dameng.com)
 * @brief
 * @date 2023-11-25
 *
 */

#pragma once
#include "Common.h"
#include "Debug.h"

#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>

namespace dm {

class FileStepLineParser
{
private:
    FILE* mInput{nullptr};   // input file handler

    char*   mReadBuffer;
    // this size should at least enough for a whole request, i.e. 21 * 1000
    static constexpr u32 mReadBufferSize = 2 * 1024 * 1024;

    u32     mRead{0};
    u32     mParsedOffset{0};

    u64     mTotalRead{0};
    u64     mTotalLen{0};

    char*   mOutBuffer{nullptr};
    const u32   mOutBufferSize;

public:
    FileStepLineParser(char *out_buffer, u32 out_buffer_size)
    :
    mReadBuffer(new char[mReadBufferSize]),
    mOutBuffer(out_buffer),
    mOutBufferSize(out_buffer_size)
    {
        assert_nr(mOutBufferSize >= (1600 + 1) * sizeof(u64));
    }

    ~FileStepLineParser()
    {
        if (mInput)
        {
            fclose(mInput);
            mInput = nullptr;
        }
        delete[] mReadBuffer;
    }

    bool setFile(FILE *file, const u64 start_off)
    {
        if (mInput)
        {
            fclose(mInput);
            mInput = nullptr;
        }

        assert_r(file, false);
        mInput = file;

        mRead = 0;
        mParsedOffset = 0;
        mTotalRead = start_off;

        fseek(mInput, 0, SEEK_END);
        u64 file_len = ftell(mInput);
        assert_r(file_len >= start_off, false);
        mTotalLen = file_len;

        fseek(mInput, start_off, SEEK_SET);
        return true;
    }

    bool parseLine()
    {
        char *start = getLine();
        if (!start)
        {
            return false;
        }

        char *end = mReadBuffer + mParsedOffset - 2;

        char *elem_end = nullptr;
        char *conv_end = nullptr;
        u64 *out_ptr = reinterpret_cast<u64 *>(mOutBuffer);

        u32 n = 0;
        while (start < end)
        {
            elem_end = find(start, end - start, ",");
            if (!elem_end)
                elem_end = end;

            errno = 0;
            u64 v = strtoul(start, &conv_end, 10);
            assert_r(errno == 0, false);
            assert_r(conv_end == elem_end, false);

            *out_ptr = v;
            out_ptr++;

            start = elem_end + 1;

            assert_r(n <= 1000, false);
            n++;
        }

        assert_r(n >= 16 && n <= 1000, false);

        return true;
    }

private:
    char *getLine()
    {
        char *start = mReadBuffer + mParsedOffset;
        char *pos = find(start, mRead - mParsedOffset, "\r\n");
        if (pos)
        {
            mParsedOffset += pos + 2 - start;
            return start;
        }

        if (!read())
            return nullptr;

        start = mReadBuffer;
        pos = find(start, mRead, "\r\n");
        if (!pos)
            return nullptr;

        mParsedOffset += pos + 2 - start;
        return start;
    }

    bool read()
    {
        moveToHead(mReadBuffer, mReadBuffer + mParsedOffset, mRead - mParsedOffset);
        mRead -= mParsedOffset;
        mParsedOffset = 0;

        u32 size = std::min((u64)mReadBufferSize - mRead, mTotalLen - mTotalRead);
        if (size == 0)
            return false;

        if (fread(mReadBuffer + mRead, size, 1, mInput) != 1)
            return false;

        mRead += size;
        mTotalRead += size;
        return true;
    }
};


}   // end of namespace dm
