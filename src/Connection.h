/**
 * @file Connection.h
 * @author Zhao Yuanfang (zhaoyuanfang@dameng.com)
 * @brief Connection using asio
 * @date 2023-12-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Common.h"
#include "Debug.h"
#include "asio/buffer.hpp"
#include "asio/error.hpp"
#include "asio/error_code.hpp"
#include "asio/io_context.hpp"
#include <memory>


namespace dm {

class IConnection
{
protected:
    asio::io_context    *mContext = nullptr;
    Ptr<Socket>         mSocket = nullptr;

public:
    IConnection(asio::io_context *context)
    :
    mContext(context),
    mSocket(std::make_shared<Socket>(mContext))
    {

    }

    IConnection(const IConnection &other) = delete;

    IConnection(IConnection &&other)
    :
    mContext(other.mContext),
    mSocket(std::move(other.mSocket))
    {

    }

    virtual ~IConnection()
    {
        close();
    }

    IConnection &operator=(const IConnection &other) = delete;
    IConnection &operator=(IConnection &&other)
    {
        close();

        mContext = other.mContext;
        mSocket = std::move(other.mSocket);
        return *this;
    }

    virtual asio::error_code connect() = 0;

    virtual asio::error_code send(const char *data, const u32 length, u32 &nsent)
    {
        assert_r(mSocket->is_open(), asio::error::basic_errors::not_connected);

        asio::error_code ec;
        nsent = asio::write(*mSocket, asio::const_buffer(data, length), ec);
        return ec;
    }

    virtual asio::error_code recv(char *dest, const u32 size, u32 &nrecv)
    {
        assert_r(mSocket->is_open(), asio::error::basic_errors::not_connected);

        asio::error_code ec;
        nrecv = asio::read(*mSocket, asio::mutable_buffer(dest, size), ec);
        return ec;
    }

    virtual void close()
    {
        if (mSocket->is_open())
        {
            mSocket->shutdown(Socket::shutdown_both);
            mSocket->close();
            mSocket = std::make_shared<Socket>(mContext);
        }
    }

    virtual String  getRemote() = 0;

    virtual Socket &getSocket() { return *mSocket; }

};  // end of class IConnection


class LocalUnixConnection : public IConnection
{
private:
    String  mRemote;

public:
    LocalUnixConnection(asio::io_context *context, const String &remote)
    :
    IConnection(context),
    mRemote(remote)
    {

    }

    bool setRemote(const String &remote)
    {
        if (mSocket)
        {
            return false;
        }

        mRemote = remote;
        return true;
    }

    virtual asio::error_code connect() override
    {
        assert_r(mRemote, asio::error::basic_errors::invalid_argument);
        close();

        asio::error_code c;

        mSocket->open(stream_protocol(AF_UNIX, 0), c);
        if (c)
        {
            mSocket = nullptr;
            return c;
        }

        auto ep = asio::local::stream_protocol::endpoint(mRemote);
        mSocket->connect(ep, c);
        if (c)
        {
            mSocket = nullptr;
        }

        return c;
    }

    virtual String getRemote() override
    {
        return mRemote;
    }

};  // end of class IConnection


class TcpConnection : public IConnection
{
private:
    String  mHost;
    String  mService = std::to_string(DM_SERVICE_PORT);

public:
    TcpConnection(asio::io_context *context, const String &host)
    :
    IConnection(context),
    mHost(host)
    {

    }

    TcpConnection(asio::io_context *context, const String &host, const String &service)
    :
    IConnection(context),
    mHost(host),
    mService(service)
    {

    }

    bool setRemote(const String &host, const String &service)
    {
        if (mSocket)
        {
            return false;
        }

        mHost = host;
        mService = service;
        if (service.empty())
        {
            mService = std::to_string(DM_SERVICE_PORT);
        }
        return true;
    }

    virtual asio::error_code connect() override
    {
        assert_r(mHost, asio::error::basic_errors::invalid_argument);
        close();

        asio::error_code ec;

        tcp::resolver resolver(*mContext);
        tcp::resolver::results_type endpoints = resolver.resolve(mHost, mService);

        mSocket->open(stream_protocol(AF_INET, IPPROTO_TCP));

        for (auto &ep : endpoints)
        {
            mSocket->connect(ep.endpoint(), ec);
            if (!ec) break;
        }
        return ec;
    }

    virtual String getRemote() override
    {
        return mHost + ":" + mService;
    }
};  // end of class IConnection


}   // end of namespace dm

