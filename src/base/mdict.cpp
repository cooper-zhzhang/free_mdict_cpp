#include "include/base/mdict.h"
#include "include/base/key_word.h"
#include "include/utils/code.h"
#include "include/utils/xml.h"
#include "include/utils/compress.h"
#include "include/utils/ripemd128.h"
#include <thread>
#include <algorithm>
#include <sstream>

// 确保正确识别C语言函数
extern "C"
{
    byte *ripemd128bytes(uint8_t *message, int length);
}
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <map>
#include <string>

#include <codecvt>
#include <locale>

namespace FreeMDict
{
    Mdict::Mdict(std::string dict_path) : dict_path_(dict_path)
    {
        std::string end_with;
        for (int i = dict_path_.size() - 1; i > 0; i--)
        {
            if (dict_path_[i] == '.')
            {
                end_with = dict_path_.substr(i, dict_path_.size() - i);
                break;
            }
        }

        if (end_with == ".mdd")
        {
            this->filetype_ = FileType::MDDTYPE;
        }
        else
        {
            this->filetype_ = FileType::MDXTYPE;
        }
        // mdd默认
        code_type_ = Utils::CodeType::UTF16LE;
    }

    Mdict::~Mdict()
    {
        // 关闭文件流
        file_.close();
        for (auto it : keyword_items_)
        {
            delete it;
        }
        keyword_items_.clear();

        for (auto it : record_blocks_)
        {
            delete it;
        }
        record_blocks_.clear();

        for (auto it : key_blocks_)
        {
            delete it;
        }
        key_blocks_.clear();
    }

    bool Mdict::init()
    {
        if (filetype_ != FileType::MDDTYPE && filetype_ != FileType::MDXTYPE)
        {
            throw std::runtime_error("filetype error: " + dict_path_);
            return false;
        }

        // 打开文件流（二进制模式）
        file_ = std::ifstream(dict_path_, std::ios::binary);
        if (!file_.is_open())
        {
            throw std::runtime_error("Dictionary file not found: " + dict_path_);
        }

        if (parseFile() != 0)
        {
            throw std::runtime_error("parse file failed : " + dict_path_);
        }
        // 有些字典中的顺序和关键词记录中的顺序不一致，需要排序
        std::sort(keyword_items_.begin(), keyword_items_.end(), [](const KeywordItem *a, const KeywordItem *b)
             { return a->keyword < b->keyword; });

        return true;
    }

    void Mdict::displayInfo()
    {
        
    }

    int Mdict::parseFile()
    {
        if (parseHead() != 0)
        {
            std::cerr << "parse head error:" <<this->err_msg_  << std::endl;
            return -1;
        }
        keyword_pos_ = file_.tellg();
        // KeyWord section
        if (parseKeywordSection() != 0)
        {
            std::cerr << "parse keyword section error:" <<this->err_msg_  << std::endl;
            return -1;
        }

        // Record section
        if (parseRecordSection() != 0)
        {
            std::cerr << "parse record section error:" <<this->err_msg_  << std::endl;
            return -1;
        }

        if (keyword_items_.size() > 0)
        {
            keyword_items_[keyword_items_.size() - 1]->record_size =
                decomp_record_blocks_size_ - keyword_items_[keyword_items_.size() - 1]->record_offset;
        }

        return 0;
    }

    int Mdict::parseHead()
    {
        // 4 Bytes
        int32_t head_size = 0;
        file_.read(reinterpret_cast<char *>(&head_size), sizeof(head_size));
        head_size = Utils::fromBigEndianInt32(head_size);
        
        if (head_size <= 0)
        {
            std::cerr << "head_size error" << std::endl;
            this->err_msg_ = "head_size error";
            return -1;
        }

        // 获取当前文件偏移量
        std::streampos current_pos = file_.tellg();

        std::string head(head_size, '\0');
        file_.read(head.data(), head_size);

        // 获取当前文件偏移量
        current_pos = file_.tellg();

        uint32_t checkSum;
        file_.read(reinterpret_cast<char *>(&checkSum), sizeof(checkSum));
        checkSum = Utils::fromLittleEndianInt32(checkSum);

        // ADLER32
        uint32_t adler32CheckSum = Utils::adler32(head);
        if (checkSum != adler32CheckSum)
        {
            std::cerr << "checkSum error" << std::endl;
            this->err_msg_ = "checkSum error";
            return -1;
        }

        std::u16string head_u16 = Utils::utf16leToU16String(head);
        std::string head_utf8 = Utils::utf16leToUtf8(head_u16);

        // 调用XML解析功能
        std::map<std::u16string, std::u16string> attributes = Utils::parseXmlHead(head_u16);
        if (attributes.empty())
        {
            std::cerr << "parseXmlHead error" << std::endl;
            this->err_msg_ = "parseXmlHead error";
            return -2;
        }

        if (attributes.find(u"<Library_Data") == attributes.end() && attributes.find(u"<Dictionary") == attributes.end())
        {
            std::cerr << "xml format error" << std::endl;
            this->err_msg_ = "xml format error";
            return -3;
        }

        if (attributes.find(u"GeneratedByEngineVersion") == attributes.end() || attributes[u"GeneratedByEngineVersion"] != u"2.0")
        {
            std::cerr << "no support version" << Utils::utf16leToUtf8(attributes[u"GeneratedByEngineVersion"]) << std::endl;
            this->err_msg_ = "no support version";
            return -4;
        }
        else
        {
            this->version_ = "v2.0";
        }

        if (attributes.find(u"Encoding") != attributes.end())
        {
            if (attributes[u"Encoding"] == u"UTF-8")
            {
                code_type_ = Utils::CodeType::UTF8;
            }
            else if (attributes[u"Encoding"] == u"UTF-16")
            {
                code_type_ = Utils::CodeType::UTF16LE;
            }
            else if (attributes[u"Encoding"] == u"")
            {
                code_type_ = Utils::CodeType::UTF16LE;
            }
            else
            {
                std::cerr << "no support encoding" << Utils::utf16leToUtf8(attributes[u"Encoding"]) << std::endl;
                this->err_msg_ = "no support encoding";
                return -1;
            }
        }

        if (attributes.find(u"Encrypted") != attributes.end())
        {
            std::string encrypted = Utils::utf16leToUtf8(attributes[u"Encrypted"]);
            if (!encrypted.empty() && encrypted != "No")
            {
                int Encrypted = std::stoi(encrypted);
                if (Encrypted != 0)
                {
                    this->encrypted_ = Encrypted;
                }
            }
        }

        return 0;
    }

    int Mdict::parseKeywordSection()
    {
        if (encrypted_ & 1)
        {
            std::cerr << "no support keyword block encrypt" << std::endl;
            this->err_msg_ = "no support keyword block encrypt";
            return -1;
        }


        const int keyword_header_count = 5;
        const int keyword_header_size = keyword_header_count * sizeof(uint64_t);
        u_char keyword_header[keyword_header_size];
        file_.read(reinterpret_cast<char *>(keyword_header), keyword_header_size);

        // ADLER32
        uint32_t adler32CheckSum = Utils::adler32(keyword_header, keyword_header_size);
        uint32_t checksum;

        file_.read(reinterpret_cast<char *>(&checksum), sizeof(checksum));
        checksum = Utils::fromBigEndianUInt32(checksum);

        if (adler32CheckSum != checksum)
        {
            std::cerr << "checksum error" << std::endl;
            this->err_msg_ = "checksum error";
            return -5;
        }

        key_blocks_num_ = Utils::fromBigEndianUInt64(keyword_header + sizeof(uint64_t) * 0, sizeof(uint64_t));
        num_entries_ = Utils::fromBigEndianUInt64(keyword_header + sizeof(uint64_t) * 1, sizeof(uint64_t));
        key_index_decomp_len_ = Utils::fromBigEndianUInt64(keyword_header + sizeof(uint64_t) * 2, sizeof(uint64_t));
        key_index_comp_len_ = Utils::fromBigEndianUInt64(keyword_header + sizeof(uint64_t) * 3, sizeof(uint64_t));
        key_blocks_len_ = Utils::fromBigEndianUInt64(keyword_header + sizeof(uint64_t) * 4, sizeof(uint64_t));

        if (parseKeywordIndex() != 0)
        {
            std::cerr << "parse keyword index error:" <<this->err_msg_  << std::endl;
            return -1;
        }

        if (parseKeywordBlocks() != 0) // key 和offset
        {
            std::cerr << "parse keyword block error:" <<this->err_msg_  << std::endl;
            return -1;
        }

        return 0;
    }

    // 读取压缩数据
    u_char *Mdict::read_compress_data(uint64_t key_index_comp_len, uint64_t key_index_decomp_len, bool decrypt, bool use_zlib)
    {
        uint32_t comp_type;
        file_.read(reinterpret_cast<char *>(&comp_type), sizeof(comp_type));
        // 直接对比字节位
       if (comp_type == Utils::ZLIB_COMP_TYPE)
        {

        }else if (comp_type == Utils::NO_COMP_TYPE)
        {
            this->err_msg_ = "not support NO_COMP_TYPE";
            return nullptr;
        }
        else if (comp_type == Utils::LZO_COMP_TYPE)
        {
            this->err_msg_ = "not support LZO_COMP_TYPE";
            return nullptr;
        }
        else
        {
            this->err_msg_ = "not support comp type";
            return nullptr;
        }

        u_char key_source[8] = {0x0, 0x0, 0x0, 0x0, 0x95, 0x36, 0x00, 0x00};
        uint32_t checksum;
        file_.read(reinterpret_cast<char *>(&checksum), sizeof(checksum));
        memcpy(key_source, &checksum, sizeof(checksum)); // 0-3
        checksum = Utils::fromBigEndianUInt32(checksum);
        
        key_index_comp_len = key_index_comp_len - sizeof(comp_type) - sizeof(checksum);

        u_char *key_index_comp = new u_char[key_index_comp_len + 1];
        key_index_comp[key_index_comp_len] = 0;
        file_.read(reinterpret_cast<char *>(key_index_comp), key_index_comp_len);
        if (decrypt)
        {
            byte *key = ripemd128bytes(key_source, 8);
            if (key != nullptr)
            {
                Utils::decrypt(key_index_comp, key_index_comp_len, reinterpret_cast<u_char *>(key), 16);
            }
            else
            {
                this->err_msg_ = "RIPEMD-128 hash generate failed";
                delete[] key_index_comp;
                return nullptr;
            }
        }

        u_char *key_index_decomp = nullptr;
        if (comp_type == Utils::ZLIB_COMP_TYPE) // TODO: 扩充压缩，使用继承-注册的形式
        {
            key_index_decomp = Utils::zlibDecompress(key_index_comp, key_index_comp_len, key_index_decomp_len);
            delete[] key_index_comp;
            if (key_index_decomp == nullptr)
            {
                this->err_msg_ = "zlib decompress failed";
                return nullptr;
            }
            uint32_t adler32CheckSum = Utils::adler32(key_index_decomp, key_index_decomp_len);
            if (adler32CheckSum != checksum)
            {
                delete[] key_index_decomp;
                std::cerr << "adler32 checksum error: " << adler32CheckSum << " != " << checksum << std::endl;
                return nullptr;
            }
            return key_index_decomp;
        }

        delete[] key_index_comp;

        return nullptr;
    }

    int Mdict::parseKeywordIndex()
    {
        u_char *key_index_decomp = read_compress_data(key_index_comp_len_, key_index_decomp_len_, encrypted_ & 2);
        if (key_index_decomp == nullptr)
        {
            std::cerr << "read_compress_data failed: " << this->err_msg_ << std::endl;
            return -1;
        }

        int pos = 0;
        for (size_t i = 0; i < key_blocks_num_; ++i)
        {
            uint64_t keywords_num = Utils::fromBigEndianUInt64(key_index_decomp + pos, sizeof(uint64_t)); // Number of keywords in the first keyword block.
            pos += sizeof(uint64_t);

            uint16_t first_word_len = Utils::fromBigEndianUInt16(key_index_decomp + pos, sizeof(uint16_t)); // Length of first_word[0], not including trailing null character. In number of “basic units” for the encoding, so e.g. bytes for UTF-8, and 2-byte units for UTF-16.
            pos += sizeof(uint16_t);
            first_word_len += 1; // 包含 trailing null character
            first_word_len *= getCodeTypeSize(code_type_);

            std::string first_word_utf8;
            if (code_type_ == Utils::CodeType::UTF16LE)
            {
                first_word_utf8 = Utils::utf16leToUtf8((char16_t *)(key_index_decomp + pos), (char16_t *)(key_index_decomp + pos + first_word_len));
            }
            else if (code_type_ == Utils::CodeType::UTF8)
            {
                first_word_utf8 = std::string((char *)(key_index_decomp + pos), first_word_len);
            }

            pos += first_word_len;
            uint16_t last_word_len = Utils::fromBigEndianUInt16(key_index_decomp + pos, sizeof(uint16_t)); // Length of last_word[0], not including trailing null character. In number of “basic units” for the encoding, so e.g. bytes for UTF-8, and 2-byte units for UTF-16.
            pos += sizeof(uint16_t);

            last_word_len += 1; // 包含 trailing null character
            last_word_len *= getCodeTypeSize(code_type_);
            
            std::string last_word_utf8;
            if (code_type_ == Utils::CodeType::UTF16LE)
            {
                last_word_utf8 = Utils::utf16leToUtf8((char16_t *)(key_index_decomp + pos), (char16_t *)(key_index_decomp + pos + last_word_len - getCodeTypeSize(code_type_))); // 不要最后的null character
            }
            else if (code_type_ == Utils::CodeType::UTF8)
            {
                last_word_utf8 = std::string((char *)(key_index_decomp + pos), last_word_len - getCodeTypeSize(code_type_));
            }
            pos += last_word_len;
            
            uint64_t comp_size = Utils::fromBigEndianUInt64(key_index_decomp + pos, sizeof(uint64_t));
            pos += sizeof(uint64_t);

            uint64_t decomp_size = Utils::fromBigEndianUInt64(key_index_decomp + pos, sizeof(uint64_t));
            pos += sizeof(uint64_t);

            KeyBlock *block_ptr = new KeyBlock(first_word_utf8, last_word_utf8, static_cast<uint32_t>(decomp_size),
                                               static_cast<uint32_t>(comp_size), static_cast<uint32_t>(keywords_num));
            key_blocks_.push_back(block_ptr);
        }

        delete[] key_index_decomp;

        return 0;
    }

    int Mdict::parseKeywordBlock(int i)
    {
        if (i >= key_blocks_.size())
        {
            return -1;
        }

        KeyBlock *block = key_blocks_[i];
        u_char *key_index_decomp = read_compress_data(block->comp_size, block->decomp_size);
        if (key_index_decomp == nullptr)
        {
            std::cerr << "read_compress_data failed: " << this->err_msg_ << std::endl;
            return -1;
        }

        KeywordItem *pre_item = nullptr;
        uint pos = 0;
        uint keywords_num = block->keywords_num;
        for (int j = 0; j < keywords_num; ++j)
        {
            uint64_t keyword_offset = Utils::fromBigEndianUInt64(key_index_decomp + pos, sizeof(uint64_t));
            pos += sizeof(uint64_t);

            uint64_t begin_pos = pos;
            uint64_t end_pos = 0;
            std::string keyword_utf8;
            if (code_type_ == Utils::CodeType::UTF16LE)
            {
                uint64_t off = Utils::check_null<char16_t>(key_index_decomp + pos, block->decomp_size - pos);
                pos += off;
                end_pos = pos - sizeof(char16_t); // ignore null character
                keyword_utf8 = Utils::utf16leToUtf8((char16_t *)(key_index_decomp + begin_pos), (char16_t *)(key_index_decomp + end_pos));
            }
            else if (code_type_ == Utils::CodeType::UTF8)
            {
                uint64_t off = Utils::check_null<char>(key_index_decomp + pos, block->decomp_size - pos);
                pos += off;
                end_pos = pos - sizeof(char);
                keyword_utf8 = std::string((char *)(key_index_decomp + begin_pos), end_pos - begin_pos);
            }

            KeywordItem *item = new KeywordItem(std::move(keyword_utf8), keyword_offset);
            keyword_items_.push_back(item);
            if (pre_item != nullptr)
            {
                pre_item->record_size = item->record_offset - pre_item->record_offset;
            }
            pre_item = item;
        }
        delete[] key_index_decomp;

        return 0;
    }

    int Mdict::parseKeywordBlocks()
    {
        keyword_blocks_begin_pos_ = file_.tellg();

#ifdef SMALL_MEMORY
        // 这部分数据是内存大头。小内存下不读取
        uint64_t offset = 0;
        for (int i = 0; i < key_blocks_.size(); ++i)
        {
            offset += key_blocks_[i]->comp_size;
        }

        file_.seekg(offset + keyword_blocks_begin_pos_);
        return 0;
#endif // SMALL_MEMORY

        keyword_items_.reserve(num_entries_);
        for (size_t i = 0; i < key_blocks_num_; ++i)
        {
            if (parseKeywordBlock(i) != 0)
            {
                return -1;
            }
        }

        return 0;
    }

    int Mdict::parseRecordSection()
    {
        uint64_t num_blocks = 0;
        file_.read(reinterpret_cast<char *>(&num_blocks), sizeof(num_blocks));
        num_blocks = Utils::fromBigEndianUInt64(num_blocks);

        uint64_t num_entries = 0;
        file_.read(reinterpret_cast<char *>(&num_entries), sizeof(num_entries));
        num_entries = Utils::fromBigEndianUInt64(num_entries);
        if (num_entries != num_entries_)
        {
#ifndef SMALL_MEMORY
            std::cerr << "RecordSection num_entries != "<< num_entries << "keyword num_entries: " << num_entries_ << std::endl;
            return -1;
#endif // SMALL_MEMORY
        }

        uint64_t index_len = 0;
        file_.read(reinterpret_cast<char *>(&index_len), sizeof(index_len));
        index_len = Utils::fromBigEndianUInt64(index_len);

        uint64_t blocks_len = 0;
        file_.read(reinterpret_cast<char *>(&blocks_len), sizeof(blocks_len));
        blocks_len = Utils::fromBigEndianUInt64(blocks_len);

        u_char *key_index_data = new u_char[index_len];
        file_.read(reinterpret_cast<char *>(key_index_data), index_len);

        std::vector<std::pair<uint64_t, uint64_t>> block_infos;
        uint64_t com_record_begin_pos = 0;
        uint64_t decomp_record_begin_pos = 0;

        uint64_t pos = 0;
        for (int i = 0; i < num_blocks; ++i)
        {
            uint64_t comp_size = Utils::fromBigEndianUInt64(key_index_data + pos, sizeof(uint64_t)); // Compressed size of the keyword block.
            pos += sizeof(uint64_t);
            uint64_t decomp_size = Utils::fromBigEndianUInt64(key_index_data + pos, sizeof(uint64_t)); // Decompressed size of the keyword block.
            pos += sizeof(uint64_t);
            block_infos.push_back({comp_size, decomp_size});
            RecordBlockInfo *record_block_info = new RecordBlockInfo(comp_size, decomp_size, nullptr,
                                                                     decomp_record_begin_pos, decomp_record_begin_pos + decomp_size, com_record_begin_pos, com_record_begin_pos + comp_size);
            record_blocks_.push_back(record_block_info);

            decomp_record_begin_pos += decomp_size;
            com_record_begin_pos += comp_size;
        }

        com_record_blocks_size_ = com_record_begin_pos;
        decomp_record_blocks_size_ = decomp_record_begin_pos;

        record_block_begin_pos_ = file_.tellg();

        delete[] key_index_data;

        return 0;
    }

    KeywordItem Mdict::getKeyWord(const std::string &key)
    {

#if SMALL_MEMORY

        uint64_t offset = 0;
        for (int i = 0; i < key_blocks_.size(); ++i)
        {
            if (key < key_blocks_[i]->first_key || key > key_blocks_[i]->last_key)
            {
                offset += key_blocks_[i]->comp_size;
                continue;
            }

            file_.seekg(offset + keyword_blocks_begin_pos_);
            if (parseKeywordBlock(i) != 0)
            {
                return KeywordItem("", 0);
            }
            else
            {
                sort(keyword_items_.begin(), keyword_items_.end(), [](const KeywordItem *a, const KeywordItem *b)
                     { return a->keyword < b->keyword; });
                break;
            }
        }
#endif // SMALL_MEMORY

        // 使用二分查找代替线性查找，提高查找效率（假设keyword_items_已按keyword_utf8排序）
        int left = 0;
        int right = keyword_items_.size() - 1;
        bool found = false;
        int mid = 0;
        
        while (left <= right)
        {
            mid = left + (right - left) / 2; // 避免整数溢出
            const std::string &mid_keyword = keyword_items_[mid]->keyword;

            int cmp_result = mid_keyword.compare(key);
            if (cmp_result == 0)
            {
                found = true;
                break;
            }
            else if (cmp_result < 0)
            {
                left = mid + 1;
            }
            else
            {
                right = mid - 1;
            }
        }

        KeywordItem item("", 0);
        if (found)
        {
            item = *(keyword_items_[mid]);
        }

#ifdef SMALL_MEMORY
        clear_up_keyword_items();
#endif

        return item;
    }

    void Mdict::clear_up_keyword_items()
    {
        // 小内存下，需要释放keyword_items_
        for (int i = 0; i < keyword_items_.size(); ++i)
        {
            delete keyword_items_[i];
        }
        keyword_items_.clear();
    }

    RecordBlockInfo *Mdict::getRecordBlockInfo(uint64_t record_offset)
    {
        auto record_block_it = std::find_if(record_blocks_.begin(), record_blocks_.end(), [record_offset](const RecordBlockInfo *block)
                                            { return block->decomp_record_begin_offset <= record_offset &&
                                                     record_offset < block->decomp_record_begin_offset + block->decomp_size; });

        return record_block_it == record_blocks_.end() ? nullptr : *record_block_it;
    }

    std::string Mdict::lookup(std::string &key)
    {
        if (filetype_ != FileType::MDXTYPE)
        {
            return "";
        }

        std::stringstream ss;
        if (!getResourceByKey(key, ss))
        {
            return "";
        }
        std::string str = ss.str();

        if (code_type_ == Utils::CodeType::UTF8)
        {
            return std::string(str.c_str(), str.length() - Utils::getCodeTypeSize(Utils::CodeType::UTF8));
        }
        else if (code_type_ == Utils::CodeType::UTF16LE)
        {
            return Utils::utf16leToUtf8((char16_t *)str.c_str(), (char16_t *)str.c_str() + str.length() - Utils::getCodeTypeSize(Utils::CodeType::UTF16LE));
        }

        return str;
    }

    const char* Mdict::getResourceByKey(const std::string &key, int &len)
    {
        // 1 查找关键词项
        KeywordItem keyword_item = getKeyWord(key);
        if (keyword_item.keyword.empty())
        {
            return nullptr;
        }

        RecordBlockInfo *record_block = getRecordBlockInfo(keyword_item.record_offset);
        if (record_block == nullptr)
        {
            return nullptr;
        }

        // 3 提取记录
        file_.seekg(record_block->com_record_begin_offset + record_block_begin_pos_);
        auto decompressed_data = read_compress_data(record_block->comp_size, record_block->decomp_size);
        if (decompressed_data == 0)
        {
            std::cerr << "decompress record failed" << std::endl;
            return nullptr;
        }

        auto begin_pos = decompressed_data + (keyword_item.record_offset - record_block->decomp_record_begin_offset);
        len = keyword_item.record_size;
        char *result = new char[keyword_item.record_size];
        memcpy(result, begin_pos, keyword_item.record_size);

        delete[] decompressed_data;

        return result;
    }

    bool Mdict::getResourceByKey(std::string key, std::ostream &out)
    {
        int len = 0;
        const char* result = getResourceByKey(key, len);
        if (result == nullptr)
        {
            return false;
        }
        out.write(result, len);

        delete[] result;

        return true;
    }

    const std::vector<KeywordItem *> &Mdict::getKeywordItems() const
    {
        return keyword_items_;
    }

    const char* Mdict::locate(const std::string &key, int &len){
        return getResourceByKey(key, len);
    }

} // namespace FreeMDict
