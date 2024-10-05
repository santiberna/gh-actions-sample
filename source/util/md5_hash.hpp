#pragma once

#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <array>

union Hash128
{
    std::array<uint8_t, 16> as_bytes;
    std::array<uint32_t, 4> as_uints;
    std::array<uint64_t, 2> as_ulongs;
};

std::vector<uint8_t> PadMessage(const std::vector<uint8_t>& str);
std::string FormatHash(const Hash128& hash);
Hash128 MD5Hash(const std::vector<uint8_t>& input);
void MD5Transform(std::array<uint32_t, 4>& state, const uint8_t* buffer);
std::vector<uint8_t> StringToBytes(const std::string& str);

std::string OldHash(std::string);
