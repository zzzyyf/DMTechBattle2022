#pragma once
#include <cstdint>
#include <cstdio>
#include <string>
#include <cassert>
#include "asio/asio.hpp"

namespace dm {
using u8 = uint8_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
using String = std::string;
using asio::ip::tcp;
using asio::generic::stream_protocol;
using socket = asio::generic::stream_protocol::socket;

}   // end of namespace dm;
