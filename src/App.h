/**
 * @file App.h
 * @author Zhao Yuanfang (zhaoyuanfang@dameng.com)
 * @brief provide argument parsing and signal handling ability
 * @date 2023-12-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Common.h"
#include "asio/executor_work_guard.hpp"
#include "asio/io_context.hpp"

namespace dm {

class App
{
private:
    static asio::io_context mContext;
    static asio::executor_work_guard<asio::io_context::executor_type> mContextGuard;

public:
    static bool RegisterSignalHandler(i32 signal, void (*handler)(int));

    static i32 Start(i32 argc, char *argv[]);

    static void OnExit(i32 _);

    static asio::io_context *GetContext() { return &mContext; }
};


}   // end of namespace dm
