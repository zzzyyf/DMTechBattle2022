/**
 * @file Receiver.h
 * @author Zhao Yuanfang (zhaoyuanfang@dameng.com)
 * @brief
 * @date 2023-12-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Connection.h"
#include "asio/error_code.hpp"
#include "asio/ip/address.hpp"
#include "asio/local/stream_protocol.hpp"
#include <memory>

namespace dm {

template <class Func>
class TcpReceiver
{
private:
    asio::io_context       *mContext = nullptr;

    String  mHost;
    u16     mPort;

    asio::ip::tcp::acceptor mAcceptor;

    Func && mHandler;

public:
    TcpReceiver(asio::io_context *context, const String &host, const u16 port, Func &&func)
    :
    mContext(context),
    mHost(host),
    mPort(port),
    mAcceptor(*context, tcp::endpoint(asio::ip::make_address(mHost), mPort)),
    mHandler(func)
    {
        startAccept();
    }

    TcpReceiver(asio::io_context *context, const u16 port, Func &&func)
    :
    mContext(context),
    mPort(port),
    mAcceptor(*context, tcp::endpoint(tcp::v4(), mPort)),
    mHandler(func)
    {
        startAccept();
    }

private:
    void startAccept()
    {
        Ptr<TcpConnection> conn = std::shared_ptr<TcpConnection>();
        mAcceptor.async_accept(conn->getSocket(), [&](const asio::error_code &ec) {
            if (ec)
            {
                std::cout << "Failed to accept: " << ec.message() << std::endl;
                return;
            }

            mHandler(std::move(conn));

            startAccept();
        });
    }

};  // end of class TcpReceiver


template <class Func>
class LocalReceiver
{
private:
    asio::io_context       *mContext = nullptr;

    String  mHost;

    asio::local::stream_protocol::acceptor mAcceptor;

    Func && mHandler;

public:
    LocalReceiver(asio::io_context *context, const String &host, Func &&func)
    :
    mContext(context),
    mHost(host),
    mAcceptor(*context, asio::local::stream_protocol::endpoint(mHost)),
    mHandler(func)
    {
        startAccept();
    }

private:
    void startAccept()
    {
        Ptr<LocalUnixConnection> conn = std::shared_ptr<LocalUnixConnection>();
        mAcceptor.async_accept(conn->getSocket(), [&](const asio::error_code &ec) {
            if (ec)
            {
                std::cout << "Failed to accept: " << ec.message() << std::endl;
                return;
            }

            mHandler(std::move(conn));

            startAccept();
        });
    }

};  // end of class LocalReceiver

}   // end of namespace dm;