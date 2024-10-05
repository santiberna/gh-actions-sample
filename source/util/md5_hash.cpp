#include "md5_hash.hpp"
#include <array>
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <cstddef>

#include "fmt/format.h"
#include "format.hpp"

constexpr std::array<uint8_t, 64> PADDING = { 0x80 };

constexpr std::array<uint32_t, 64> S = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

constexpr std::array<uint32_t, 64> K = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

constexpr std::array<uint32_t, 4> I = {
    0x67452301,
    0xefcdab89,
    0x98badcfe,
    0x10325476
};

#define FF(a, b, c, d, x, s, ac)              \
    {                                         \
        a += ((b & c) | ((~b) & d)) + x + ac; \
        a = b + ((a << s) | (a >> (32 - s))); \
    }

#define GG(a, b, c, d, x, s, ac)              \
    {                                         \
        a += ((b & d) | (c & (~d))) + x + ac; \
        a = b + ((a << s) | (a >> (32 - s))); \
    }

#define HH(a, b, c, d, x, s, ac)              \
    {                                         \
        a += (b ^ c ^ d) + x + ac;            \
        a = b + ((a << s) | (a >> (32 - s))); \
    }

#define II(a, b, c, d, x, s, ac)              \
    {                                         \
        a += (c ^ (b | (~d))) + x + ac;       \
        a = b + ((a << s) | (a >> (32 - s))); \
    }

std::vector<uint8_t> StringToBytes(const std::string& str)
{
    std::vector<uint8_t> out;
    out.resize(str.size());
    std::memcpy(out.data(), str.data(), str.size());
    return out;
}

std::vector<uint8_t> PadMessage(const std::vector<uint8_t>& str)
{
    std::vector<uint8_t> out = str;

    size_t const byte_length = str.size();
    int necessary_padding = 56 - (static_cast<int>(byte_length) % 64);
    if (necessary_padding < 0) {
        necessary_padding += 64;
}

    out.resize(out.size() + necessary_padding + 8, 0);
    out.at(byte_length) = 0x80;

    void* last_8_bytes = out.data() + out.size() - 8;
    size_t bit_length = byte_length * 8;

    std::memcpy(last_8_bytes, &bit_length, 8);

    return out;
}

void MD5Transform(std::array<uint32_t, 4>& state, const uint8_t* buffer)
{
    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    std::array<uint32_t, 16> x {};

    for (size_t i = 0, j = 0; j < 64; ++i, j += 4)
    {
        x[i] = static_cast<uint32_t>(buffer[j]) | (static_cast<uint32_t>(buffer[j + 1]) << 8) | (static_cast<uint32_t>(buffer[j + 2]) << 16) | (static_cast<uint32_t>(buffer[j + 3]) << 24);
    }

    /* Round 1 */

    FF(a, b, c, d, x[0], S[0], K[0]);
    FF(d, a, b, c, x[1], S[1], K[1]);
    FF(c, d, a, b, x[2], S[2], K[2]);
    FF(b, c, d, a, x[3], S[3], K[3]);
    FF(a, b, c, d, x[4], S[4], K[4]);
    FF(d, a, b, c, x[5], S[5], K[5]);
    FF(c, d, a, b, x[6], S[6], K[6]);
    FF(b, c, d, a, x[7], S[7], K[7]);
    FF(a, b, c, d, x[8], S[8], K[8]);
    FF(d, a, b, c, x[9], S[9], K[9]);
    FF(c, d, a, b, x[10], S[10], K[10]);
    FF(b, c, d, a, x[11], S[11], K[11]);
    FF(a, b, c, d, x[12], S[12], K[12]);
    FF(d, a, b, c, x[13], S[13], K[13]);
    FF(c, d, a, b, x[14], S[14], K[14]);
    FF(b, c, d, a, x[15], S[15], K[15]);
    /* Round 2 */

    GG(a, b, c, d, x[1], S[16], K[16]);
    GG(d, a, b, c, x[6], S[17], K[17]);
    GG(c, d, a, b, x[11], S[18], K[18]);
    GG(b, c, d, a, x[0], S[19], K[19]);
    GG(a, b, c, d, x[5], S[20], K[20]);
    GG(d, a, b, c, x[10], S[21], K[21]);
    GG(c, d, a, b, x[15], S[22], K[22]);
    GG(b, c, d, a, x[4], S[23], K[23]);
    GG(a, b, c, d, x[9], S[24], K[24]);
    GG(d, a, b, c, x[14], S[25], K[25]);
    GG(c, d, a, b, x[3], S[26], K[26]);
    GG(b, c, d, a, x[8], S[27], K[27]);
    GG(a, b, c, d, x[13], S[28], K[28]);
    GG(d, a, b, c, x[2], S[29], K[29]);
    GG(c, d, a, b, x[7], S[30], K[30]);
    GG(b, c, d, a, x[12], S[31], K[31]);
    /* Round 3 */

    HH(a, b, c, d, x[5], S[32], K[32]);
    HH(d, a, b, c, x[8], S[33], K[33]);
    HH(c, d, a, b, x[11], S[34], K[34]);
    HH(b, c, d, a, x[14], S[35], K[35]);
    HH(a, b, c, d, x[1], S[36], K[36]);
    HH(d, a, b, c, x[4], S[37], K[37]);
    HH(c, d, a, b, x[7], S[38], K[38]);
    HH(b, c, d, a, x[10], S[39], K[39]);
    HH(a, b, c, d, x[13], S[40], K[40]);
    HH(d, a, b, c, x[0], S[41], K[41]);
    HH(c, d, a, b, x[3], S[42], K[42]);
    HH(b, c, d, a, x[6], S[43], K[43]);
    HH(a, b, c, d, x[9], S[44], K[44]);
    HH(d, a, b, c, x[12], S[45], K[45]);
    HH(c, d, a, b, x[15], S[46], K[46]);
    HH(b, c, d, a, x[2], S[47], K[47]);
    /* Round 4 */

    II(a, b, c, d, x[0], S[48], K[48]);
    II(d, a, b, c, x[7], S[49], K[49]);
    II(c, d, a, b, x[14], S[50], K[50]);
    II(b, c, d, a, x[5], S[51], K[51]);
    II(a, b, c, d, x[12], S[52], K[52]);
    II(d, a, b, c, x[3], S[53], K[53]);
    II(c, d, a, b, x[10], S[54], K[54]);
    II(b, c, d, a, x[1], S[55], K[55]);
    II(a, b, c, d, x[8], S[56], K[56]);
    II(d, a, b, c, x[15], S[57], K[57]);
    II(c, d, a, b, x[6], S[58], K[58]);
    II(b, c, d, a, x[13], S[59], K[59]);
    II(a, b, c, d, x[4], S[60], K[60]);
    II(d, a, b, c, x[11], S[61], K[61]);
    II(c, d, a, b, x[2], S[62], K[62]);
    II(b, c, d, a, x[9], S[63], K[63]);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

Hash128 MD5Hash(const std::vector<uint8_t>& input)
{
    auto padded = PadMessage(input);
    size_t const iterations = padded.size() / 64;
    Hash128 hash {};
    hash.as_uints = I;

    for (size_t i = 0; i < iterations; i++)
    {
        size_t const buffer_start = i * 64;
        MD5Transform(hash.as_uints, &padded.at(buffer_start));
    }

    return hash;
}

std::string FormatHash(const Hash128& hash)
{
    std::string out {};
    for (auto n : hash.as_bytes)
    {
        out += std::string(fmt::format("{:02x}", n));
    }
    return out;
}
