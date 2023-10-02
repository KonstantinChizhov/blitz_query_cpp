#pragma once
#include <stdint.h>
#include <string_view>
#include <string>

namespace blitz_query_cpp
{
    template <class CrcClass>
    inline constexpr typename CrcClass::ResultType CrcTable(typename CrcClass::ResultType v)
    {
        typedef typename CrcClass::ResultType T;
        const T Msb = T(1) << (sizeof(T) * 8 - 1);
        uint8_t i = 8;
        if (CrcClass::RefIn)
        {
            do
                v = v & 1 ? (v >> 1) ^ CrcClass::RevPoly : v >> 1;
            while (--i);
        }
        else
        {
            v = v << (CrcClass::Width - 8);
            do
                v = v & Msb ? (v << 1) ^ CrcClass::Poly : v << 1;
            while (--i);
        }

        return v;
    }

    template <class CrcClass>
    inline constexpr typename CrcClass::ResultType CrcUpdate(
        char newchar,
        typename CrcClass::ResultType crcval)
    {
        if (CrcClass::RefOut)
        {
            return (crcval >> 8) ^ CrcClass::Table((crcval ^ newchar) & 0xff);
        }
        else
        {
            return CrcClass::Table(newchar ^ (crcval >> (CrcClass::Width - 8))) ^ (crcval << 8);
        }
    }

    template <class CrcClass>
    inline constexpr typename CrcClass::ResultType ComputeCrc(
        const char *message,
        size_t length,
        typename CrcClass::ResultType crc = CrcClass::Init)
    {
        for (size_t i = 0; i < length; i++)
        {
            crc = CrcUpdate<CrcClass>(message[i], crc);
        }
        return crc ^ CrcClass::XorOut;
    }

    constexpr uint32_t ReverseBits(uint32_t arg)
    {
        const uint32_t t1 = ((arg >> 1) & 0x55555555) | ((arg & 0x55555555) << 1);
        const uint32_t t2 = ((t1 >> 2) & 0x33333333) | ((t1 & 0x33333333) << 2);
        const uint32_t t3 = ((t2 >> 4) & 0x0F0F0F0F) | ((t2 & 0x0F0F0F0F) << 4);
        const uint32_t t4 = ((t3 >> 8) & 0x00FF00FF) | ((t3 & 0x00FF00FF) << 8);
        const uint32_t value = (t4 >> 16) | (t4 << 16);
        return value;
    }

    struct Crc32
    {
        typedef uint32_t ResultType;
        static constexpr unsigned Width = 32;
        static constexpr ResultType Poly = 0x04C11DB7UL;
        static constexpr ResultType Init = 0XFFFFFFFFUL;
        static constexpr bool RefIn = true;
        static constexpr bool RefOut = true;
        static constexpr ResultType XorOut = 0XFFFFFFFFUL;
        static constexpr ResultType Check = 0xCBF43926UL;
        static inline constexpr const char *CheckMessage() { return "123456789"; }
        static inline constexpr const char *Name() { return "CRC 32"; }
        static constexpr ResultType RevPoly = ReverseBits(Poly);
        static inline constexpr ResultType Table(ResultType v) { return CrcTable<Crc32>(v); }
    };

    constexpr uint32_t hash_crc32(const std::string_view str)
    {
        return ComputeCrc<Crc32>(str.data(), str.size());
    }

    constexpr uint32_t operator ""_crc32(const char* str, size_t size)
    {
        return ComputeCrc<Crc32>(str, size);
    }

}