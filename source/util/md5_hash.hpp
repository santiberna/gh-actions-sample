#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>

union Hash128
{
    std::array<uint8_t, 16> m_as_bytes;
    std::array<uint32_t, 4> m_as_uints;
    std::array<uint64_t, 2> m_as_ulongs;
};

std::vector<uint8_t> pad_message(const std::vector<uint8_t>& str);
std::string format_hash(const Hash128& hash);
Hash128 m_d5_hash(const std::vector<uint8_t>& input);
void m_d5_transform(std::array<uint32_t, 4>& state, const uint8_t* buffer);
std::vector<uint8_t> string_to_bytes(const std::string& str);

std::string old_hash(std::string);
