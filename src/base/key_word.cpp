#include "include/base/key_word.h"

namespace FreeMDict
{

    // Default constructor
    KeyBlock::KeyBlock() : decomp_size(0), comp_size(0) {}
    // Constructor
    KeyBlock::KeyBlock(std::string first_key, std::string last_key,
                       uint32_t decomp_size, uint32_t comp_size,
                       uint32_t keywords_num) : first_key(first_key),
                                                last_key(last_key),
                                                decomp_size(decomp_size),
                                                comp_size(comp_size),
                                                keywords_num(keywords_num) {}
    // Copy constructor
    KeyBlock::KeyBlock(const KeyBlock &other) : first_key(other.first_key),
                                                last_key(other.last_key),
                                                decomp_size(other.decomp_size),
                                                comp_size(other.comp_size) {}

    // Constructor
    KeywordItem::KeywordItem(std::string &&keyword, uint64_t offset) : keyword(std::move(keyword)),
                                                                       record_offset(offset), record_size(0) {}

    // Move constructor
    KeywordItem::KeywordItem(KeywordItem &&other) noexcept : keyword(std::move(other.keyword)),
                                                             record_offset(other.record_offset),
                                                             record_size(other.record_size)
    {
        other.record_offset = 0;
        other.record_size = 0;
    }

    // Assignment operator
    KeywordItem &KeywordItem::operator=(KeywordItem &other) noexcept
    {
        keyword = other.keyword;
        record_offset = other.record_offset;
        record_size = other.record_size;
        return *this;
    }

    RecordBlockInfo::RecordBlockInfo(uint64_t comp_size, uint64_t decomp_size,
                                     u_char *record_block, uint64_t decomp_record_begin_pos,
                                     uint64_t decomp_record_end_pos, uint64_t com_record_begin_pos,
                                     uint64_t com_record_end_pos)
        : comp_size(comp_size),
          decomp_size(decomp_size),
          decomp_record_begin_offset(decomp_record_begin_pos),
          com_record_begin_offset(com_record_begin_pos) {}

} // namespace FreeMDict