#pragma once

#include <cstdint>
#include <string>
#include <codecvt>
#include <locale>

// 定义u_char类型为unsigned char
typedef unsigned char u_char;

namespace FreeMDict
{
    namespace Utils
    {
        /**
         * @brief 检测本机的字节序（大小端）
         * @return true 表示大端（Big Endian），false 表示小端（Little Endian）
         *
         * 大端模式：高位字节存储在低地址
         * 小端模式：低位字节存储在低地址
         */
        inline bool isBigEndian()
        {
            // 静态局部变量，只在第一次调用时初始化
            static bool result = []()
            {
                // 创建一个多字节整数
                uint16_t test_value = 0x1234;
                // 转换为字节数组
                uint8_t *bytes = reinterpret_cast<uint8_t *>(&test_value);
                // 检查第一个字节（低地址）的值
                // 如果是0x12，则是大端；如果是0x34，则是小端
                return (bytes[0] == 0x12);
            }();

            // 后续调用直接返回缓存的结果
            return result;
        }

        inline uint64_t swapEndian64(uint64_t value)
        {
            return ((value & 0x00000000000000FFULL) << 56) |
                   ((value & 0x000000000000FF00ULL) << 40) |
                   ((value & 0x0000000000FF0000ULL) << 24) |
                   ((value & 0x00000000FF000000ULL) << 8) |
                   ((value & 0x000000FF00000000ULL) >> 8) |
                   ((value & 0x0000FF0000000000ULL) >> 24) |
                   ((value & 0x00FF000000000000ULL) >> 40) |
                   ((value & 0xFF00000000000000ULL) >> 56);
        }

        inline int32_t swapEndian32(int32_t value)
        {
            uint32_t unsigned_val = static_cast<uint32_t>(value);
            uint32_t swapped = ((unsigned_val & 0x000000FF) << 24) |
                               ((unsigned_val & 0x0000FF00) << 8) |
                               ((unsigned_val & 0x00FF0000) >> 8) |
                               ((unsigned_val & 0xFF000000) >> 24);
            return static_cast<int32_t>(swapped);
        }

        inline uint32_t swap32(uint32_t value)
        {
            return ((value & 0x000000FF) << 24) |
                   ((value & 0x0000FF00) << 8) |
                   ((value & 0x00FF0000) >> 8) |
                   ((value & 0xFF000000) >> 24);
        }

        /**
         * @brief 将大端的int32转为本机字节序的int32
         * @param bigEndianValue 大端字节序的int32值
         * @return 转换为本机字节序后的int32值
         */
        inline int32_t fromBigEndianInt32(int32_t bigEndianValue)
        {
            return isBigEndian() ? bigEndianValue : swapEndian32(bigEndianValue);
        }

        inline uint32_t fromBigEndianUInt32(uint32_t bigEndianValue)
        {
            return isBigEndian() ? bigEndianValue : swapEndian32(bigEndianValue);
        }

        inline uint16_t fromBigEndianUInt16(uint16_t bigEndianValue)
        {
            return isBigEndian() ? bigEndianValue : ((bigEndianValue & 0x00FF) << 8) | ((bigEndianValue & 0xFF00) >> 8);
        }

        /**
         * @brief 将小端的int32转为本机字节序的int32
         * @param littleEndianValue 小端字节序的int32值
         * @return 转换为本机字节序后的int32值
         */
        inline int32_t fromLittleEndianInt32(int32_t littleEndianValue)
        {
            return isBigEndian() ? swapEndian32(littleEndianValue) : littleEndianValue;
        }

        inline uint32_t fromLittleEndianUInt32(uint32_t littleEndianValue)
        {
            return isBigEndian() ? swapEndian32(littleEndianValue) : littleEndianValue;
        }

        uint64_t fromBigEndianUInt64(uint64_t bigEndianValue);

        inline uint64_t fromBigEndianUInt64(const u_char *bytes, size_t size)
        {
            uint64_t *p = (uint64_t *)bytes;
            return fromBigEndianUInt64(*p);
        }

        inline uint32_t fromBigEndianUInt32(const u_char *bytes, size_t size)
        {
            uint32_t *p = (uint32_t *)bytes;
            return fromBigEndianUInt32(*p);
        }

        inline uint16_t fromBigEndianUInt16(const u_char *bytes, size_t size)
        {
            uint16_t *p = (uint16_t *)bytes;
            return fromBigEndianUInt16(*p);
        }

        inline std::u16string utf16leToU16String(const std::string &utf16leBytes)
        {
            if (utf16leBytes.size() % 2 != 0)
            {
                throw std::runtime_error("UTF-16LE bytes size must be even");
            }

            std::u16string result;
            result.resize(utf16leBytes.size() / 2);

            if (isBigEndian())
            {
                // 系统是大端，UTF-16LE 是小端，需要字节交换
                const uint8_t *src = reinterpret_cast<const uint8_t *>(utf16leBytes.data());
                char16_t *dst = result.data();

                for (size_t i = 0; i < result.size(); ++i)
                {
                    // UTF-16LE: [低字节, 高字节]
                    // 大端系统需要交换字节
                    dst[i] = (src[i * 2 + 1] << 8) | src[i * 2];
                }
            }
            else
            {
                // 系统是小端，直接内存拷贝
                std::memcpy(result.data(), utf16leBytes.data(), utf16leBytes.size());
            }

            return result;
        }

        /**
         * @brief 计算ADLER32校验和
         * @param utf16leBytes 输入的UTF-16LE字节数据
         * @return ADLER32校验和结果（uint32_t）
         *
         * ADLER32算法：
         * 1. 初始化a=1，b=0
         * 2. 对每个字节byte：
         *    a = (a + byte) % 65521
         *    b = (b + a) % 65521
         * 3. 返回 (b << 16) | a
         */
        inline uint32_t adler32(const std::string &utf16leBytes)
        {
            const uint32_t MOD_ADLER = 65521; // 最大素数小于2^16
            uint32_t a = 1;                   // 初始值
            uint32_t b = 0;

            // 遍历每个字节
            for (uint8_t byte : utf16leBytes)
            {
                a = (a + byte) % MOD_ADLER;
                b = (b + a) % MOD_ADLER;
            }

            // 组合结果
            return (b << 16) | a;
        }

        /**
         * @brief 计算ADLER32校验和（基于字节数组）
         * @param keyword_header 输入的字节数组
         * @param size 数组大小
         * @return ADLER32校验和结果（uint32_t）
         */
        inline uint32_t adler32(const u_char keyword_header[], size_t size)
        {
            const uint32_t MOD_ADLER = 65521; // 最大素数小于2^16
            uint32_t a = 1;                   // 初始值
            uint32_t b = 0;

            // 遍历每个字节
            for (size_t i = 0; i < size; ++i)
            {
                a = (a + keyword_header[i]) % MOD_ADLER;
                b = (b + a) % MOD_ADLER;
            }

            // 组合结果
            return (b << 16) | a;
        }

        enum class CodeType
        {
            UTF8 = 1,
            GBK = 2,
            BIG5 = 3,
            UTF16LE = 4,
            UTF16BE = 5,
        };

        int getCodeTypeSize(CodeType code_type);

        inline std::string utf16leToUtf8(const std::u16string &u16str)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
            return converter.to_bytes(u16str);
        }

        inline std::string utf16leToUtf8(const char16_t *s, const char16_t *e)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
            return converter.to_bytes(s, e);
        }

#define SWAPNIBBLE(byte) (((byte) >> 4) | ((byte) << 4))
        inline void decrypt(unsigned char *buf, size_t buflen, unsigned char *key, size_t keylen)
        {
            unsigned char prev = 0x36;
            unsigned char current;

            for (size_t i = 0; i < buflen; i++)
            {
                current = buf[i]; // 保存当前密文

                // 反向操作：先交换nibble，再异或
                buf[i] = SWAPNIBBLE(buf[i]);
                buf[i] = buf[i] ^ ((unsigned char)i) ^ key[i % keylen] ^ prev;

                prev = current; // 使用原始密文作为下一个prev
            }
        }

        struct TypeInfo
        {
            const std::type_info &info;
            std::string name;

            TypeInfo(const std::type_info &type)
                : info(type), name(type.name()) {}
        };


        template <typename T>
        inline uint64_t check_null(void *p, uint64_t bytesize)
        {
            if(p == nullptr)
            {
                return 0;
            }
            for(int i = 0; i < bytesize/sizeof(T); i++)
            {
                if(((T *)p)[i] == 0)
                {
                    return (i+1)*sizeof (T);
                }
            }

            return 0;
        }
    } // namespace Utils
} // namespace FreeMDict
