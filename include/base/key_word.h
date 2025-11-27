#pragma once

#include <string>
#include <cstdint>
#include <cstdint>
#include <string>
#include <codecvt>
#include <locale>
#include <iostream>

namespace FreeMDict
{

    struct KeyBlock
    {
        std::string first_key;
        std::string last_key;
        uint32_t decomp_size;
        uint32_t comp_size;
        uint32_t keywords_num;

        KeyBlock() ;
        KeyBlock(std::string first_key, std::string last_key, uint32_t decomp_size, uint32_t comp_size, uint32_t keywords_num) ;
        KeyBlock(const KeyBlock &other);
        void display(std::ostream &out_, int i = 0) const;
    };

    struct KeywordItem
    {
        KeywordItem(std::string &&keyword, uint64_t offset) ;
        KeywordItem(KeywordItem &&other) noexcept ;
        KeywordItem &operator=(KeywordItem &other) noexcept ;
        std::string keyword;
        uint64_t record_offset;
        uint64_t record_size; 
    };

        struct RecordBlockInfo
        {
            uint64_t comp_size;
            uint64_t decomp_size;
            uint64_t decomp_record_begin_offset; // Decompressed record block start position, including
            uint64_t com_record_begin_offset; // Compressed record block start position, including

            RecordBlockInfo(uint64_t comp_size, uint64_t decomp_size,
                 u_char *record_block, uint64_t decomp_record_begin_pos,
                  uint64_t decomp_record_end_pos, uint64_t com_record_begin_pos, uint64_t com_record_end_pos) ;
        };

} // namespace FreeMDict