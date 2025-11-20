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

        // 默认构造函数
        KeyBlock() : decomp_size(0), comp_size(0) {}
        // 构造函数
        KeyBlock(std::string first_key, std::string last_key, uint32_t decomp_size, uint32_t comp_size, uint32_t keywords_num) : first_key(first_key),
                                                                                                          last_key(last_key),
                                                                                                          decomp_size(decomp_size),
                                                                                                          comp_size(comp_size),
                                                                                                          keywords_num(keywords_num) {}
        // 复制构造函数
        KeyBlock(const KeyBlock &other) : first_key(other.first_key),
                                          last_key(other.last_key),
                                          decomp_size(other.decomp_size),
                                          comp_size(other.comp_size) {}

        void display(std::ostream &out_, int i = 0) const
        {
            out_ << "第" << i << "个关键词块" << std::endl;
            out_ << "first_key: " << this->first_key << std::endl;
            out_ << "last_key: " << this->last_key << std::endl;
            out_ << "decomp_size: " << this->decomp_size << std::endl;
            out_ << "comp_size: " << this->comp_size << std::endl;
            out_ << "keywords_num: " << this->keywords_num << std::endl;
        }
    };

    struct KeywordItem
    {
        // 构造函数
        KeywordItem(std::string &&keyword, uint64_t offset) : keyword(std::move(keyword)), record_offset(offset),record_size(0) {}
        
        // 移动构造函数
        KeywordItem(KeywordItem &&other) noexcept : 
            keyword(std::move(other.keyword)), 
            record_offset(other.record_offset), 
            record_size(other.record_size) {
            // 将原对象的偏移量和大小设为0，确保其处于有效但无害的状态
            other.record_offset = 0;
            other.record_size = 0;
        }

        // 赋值运算符
        KeywordItem &operator=(KeywordItem &other) noexcept {
            keyword = other.keyword;
            record_offset = other.record_offset;
            record_size = other.record_size;
            return *this;
        }
        // 关键词的utf8编码
        std::string keyword;
        // 数据的的偏移量
        uint64_t record_offset;
        uint64_t record_size; 
    };

        struct RecordBlockInfo
        {
            uint64_t comp_size;
            uint64_t decomp_size;
            //u_char *record_block; // 记录块的原始数据
            
            uint64_t decomp_record_begin_offset; //解压后的记录块的开始位置 包括
            //uint64_t decomp_record_end_pos; //解压后的记录块的开始位置 不包括

            uint64_t com_record_begin_offset; //压缩的记录块的开始位置 包括
            //uint64_t com_record_end_pos; //压缩的记录块的结束位置 不包括


            RecordBlockInfo(uint64_t comp_size, uint64_t decomp_size,
                 u_char *record_block, uint64_t decomp_record_begin_pos,
                  uint64_t decomp_record_end_pos, uint64_t com_record_begin_pos, uint64_t com_record_end_pos) : comp_size(comp_size),
                decomp_size(decomp_size),
                //record_block(record_block),
                decomp_record_begin_offset(decomp_record_begin_pos),
                //decomp_record_end_pos(decomp_record_end_pos),
                com_record_begin_offset(com_record_begin_pos)/*,
                com_record_end_pos(com_record_end_pos)*/ {}

        };

} // namespace FreeMDict