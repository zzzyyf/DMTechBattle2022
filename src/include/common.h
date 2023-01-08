#pragma once
#include "asio/asio.hpp"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

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

void moveToHead(char* buffer, char* pos, u32 len);

bool generateInput(const String &filename);

}   // end of namespace dm;
