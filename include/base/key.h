#pragma one

#include <cstdint>
#include <cstdint>
#include <codecvt>
#include <locale>

struct Key{
    std::u16string key;
    std::streampos record_pos;
    uint32_t record_size;
};