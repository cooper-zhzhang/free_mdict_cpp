#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "include/base/key_word.h"
#include "include/utils/code.h"


namespace FreeMDict {

    enum class FileType
        {
            MDDTYPE = 1,
            MDXTYPE = 2,
        };

        class Mdict {
    public:
        Mdict(std::string dict_path, std::ostream& out);
        Mdict(std::string dict_path);
        ~Mdict();
        bool init();

        // return string not include null character for mdx
        std::string lookup(std::string &key);

        // locate resource by key, return const char* include null character(if exist) for mdd/mdx
        std::string locate(const std::string &key);
        
        // locate resource by key, return const char* include null character(if exist) for mdd/mdx
        const char* locate(const std::string &key, int &len);

        //  get resource by key
        bool getResourceByKey(std::string key, std::ostream& out);

        // Check if keyword exists
        bool exists(std::string key);

        const std::vector<KeywordItem*> &getKeywordItems() const;

        void displayInfo(); // Method to display dictionary information

        void dumpAll(const std::string &dir); // Method to dump all keywords
    
    private:
        
        int parseFile(); // Private method to parse file
        int parseHead(); // Private method to parse header
        int parseKeywordSection(); // Private method to parse keywords
        int parseKeywordIndex(); // Private method to parse keyword index
        int parseKeywordBlock(int i); // Private method to parse keyword block
        int parseKeywordBlocks(); // Private method to parse keyword blocks
        int parseRecordSection(); // Private method to parse record section

        void clear_up_keyword_items();

        const char* getResourceByKey(const std::string &key, int &len);
        
        u_char * read_compress_data(uint64_t key_index_comp_len, uint64_t  key_index_decomp_len, bool decrypt=false, bool use_zlib=false);
        KeywordItem getKeyWord(const std::string &key); // 根据关键词查找关键词项
        RecordBlockInfo* getRecordBlockInfo(uint64_t record_offset); // 根据记录偏移量查找记录块信息

        std::string err_msg_ = ""; // Store error message

        std::string dict_path_ = ""; // Store dictionary file path
        FileType filetype_ = FileType::MDXTYPE; // Store file type
        std::ifstream file_; // Store file stream

        // Header related
        std::streampos head_pos_ = 0; // Store header position
        std::string version_ = ""; // Store version number
        int encrypted_ = 0; // Store encryption status
        uint32_t head_size_ = 0; // Store header size
        Utils::CodeType code_type_ = Utils::CodeType::UTF16LE;// 1: utf-8, 2: gbk, 3: big5, 4: utf-16le, 5: utf-16be
        

        // Keywords have two parts: 1) keywords index, 2) keywords blocks
        // Keywords related
        std::streampos keyword_pos_ = 0; // Store keywords position

        // Keywords index related
        uint64_t key_blocks_num_ = 0; // Number of keyword storage blocks
        uint64_t num_entries_ = 0; // Number of keywords
        uint64_t key_index_decomp_len_ = 0;  // Decompressed length 
        uint64_t key_index_comp_len_ = 0;  // Compressed length
        uint64_t key_blocks_len_ = 0;  // Total length of all keyword blocks

        // Keywords blocks related
        std::vector<KeyBlock*> key_blocks_; // Store keyword blocks
        std::vector<KeywordItem*> keyword_items_; // Store keyword items
        std::streampos keyword_blocks_begin_pos_ = 0; // Store keyword blocks position

        // Record related
         std::vector<RecordBlockInfo*> record_blocks_;
         std::streampos record_block_begin_pos_ = 0; // Store records position

        uint64_t com_record_blocks_size_ = 0; // Compressed record blocks size
        uint64_t decomp_record_blocks_size_ = 0; // Decompressed record blocks size
    };
}