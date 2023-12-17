/**
 * @file App.cpp
 * @author Zhao Yuanfang (zhaoyuanfang@dameng.com)
 * @brief
 * @date 2023-12-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "App.h"

#include <csignal>
#include <thread>

namespace dm {

asio::io_context App::mContext;
asio::executor_work_guard<asio::io_context::executor_type> App::mContextGuard = asio::make_work_guard(mContext);


/*static*/ bool
App::RegisterSignalHandler(i32 signal, void (*handler)(int))
{
    return std::signal(signal, handler) != SIG_ERR;
}

/*static*/ i32
App::Start(i32 argc, char *argv[])
{
    RegisterSignalHandler(SIGINT, &OnExit);



    mContext.run();
    return 0;
}

/*static*/ void
App::OnExit(i32 _)
{
    std::cout << "Received exit signal, exiting..." << std::endl;
    using namespace std::literals;

    mContext.stop();
    while (!mContext.stopped())
    {
        std::this_thread::sleep_for(500ms);
    }

    std::cout << "Ready to exit. Bye." << std::endl;
}


}   // end of namespace dm
