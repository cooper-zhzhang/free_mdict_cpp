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
        // // 默认构造函数
        // KeyBlock() : decomp_size(0), comp_size(0) {}
        // // 构造函数
        // KeyBlock(std::string first_key, std::string last_key, uint32_t decomp_size, uint32_t comp_size, uint32_t keywords_num) : first_key(first_key),
        //                                                                                                   last_key(last_key),
        //                                                                                                   decomp_size(decomp_size),
        //                                                                                                   comp_size(comp_size),
        //                                                                                                   keywords_num(keywords_num) {}
        // // 复制构造函数
        // KeyBlock(const KeyBlock &other) : first_key(other.first_key),
        //                                   last_key(other.last_key),
        //                                   decomp_size(other.decomp_size),
        //                                   comp_size(other.comp_size) {}

        // void display(std::ostream &out_, int i = 0) const
        // {
        //     out_ << "第" << i << "个关键词块" << std::endl;
        //     out_ << "first_key: " << this->first_key << std::endl;
        //     out_ << "last_key: " << this->last_key << std::endl;
        //     out_ << "decomp_size: " << this->decomp_size << std::endl;
        //     out_ << "comp_size: " << this->comp_size << std::endl;
        //     out_ << "keywords_num: " << this->keywords_num << std::endl;
        // }
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
            uint64_t decomp_record_begin_offset; //解压后的记录块的开始位置 包括
            uint64_t com_record_begin_offset; //压缩的记录块的开始位置 包括

            RecordBlockInfo(uint64_t comp_size, uint64_t decomp_size,
                 u_char *record_block, uint64_t decomp_record_begin_pos,
                  uint64_t decomp_record_end_pos, uint64_t com_record_begin_pos, uint64_t com_record_end_pos) ;
        };

} // namespace FreeMDict