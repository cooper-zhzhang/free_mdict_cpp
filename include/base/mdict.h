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
        ~Mdict();

        std::string lookup(std::string key);
        std::vector<std::string> allKeys();

        bool init();
        bool getResourceByKey(std::string key, std::ostream& out);

        const std::vector<KeywordItem*> &getKeywordItems() const;

        void displayInfo(); // 显示字典信息的方法
    
    private:
        std::ostream& out_; // 存储输出流
        int parseFile(); // 解析文件的私有方法
        int parseHead(); // 解析头的私有方法
        int parseKeywordSection(); // 解析关键词的私有方法
        int parseKeywordIndex(); // 解析关键词索引的私有方法
        int parseKeywordBlock(int i); // 解析关键词块的私有方法
        int parseKeywordBlocks(); // 解析关键词块的私有方法
        int parseRecordSection(); // 解析关键词记录的私有方法

        void clear_up_keyword_items();
        
        u_char * read_compress_data(uint64_t key_index_comp_len, uint64_t  key_index_decomp_len, bool decrypt=false, bool use_zlib=false);
        KeywordItem getKeyWord(const std::string &key); // 根据关键词查找关键词项


        std::string dict_path_; // 存储字典文件路径
        FileType filetype_; // 存储文件类型
        std::ifstream file_; // 存储文件流



        // header相关
        std::streampos head_pos_; // 存储头的位置
        std::string version_; // 存储版本号
        int encrypted_; // 存储是否加密
        uint32_t head_size_; // 存储头的大小
        Utils::CodeType code_type_;//  1 表示utf-8  2 表示gbk 3 表示big5 4 表示utf-16le 5 表示utf-16be
        

        // keywords分两部分，1 keywords index，2 keywords blocks
        // keywords相关
        std::streampos keyword_pos_; // 存储关键词的位置

        //  keywords index 相关
        uint64_t key_blocks_num_ ; // keywords存储块的数量
        uint64_t num_entries_ ; // keywords的数量
        uint64_t key_index_decomp_len_ ;  // 加压后的长度 
        uint64_t key_index_comp_len_ ;  // 压缩后的长度
        uint64_t key_blocks_len_;  // 所有关键词块的长度

        // keywords blocks 相关
        std::vector<KeyBlock*> key_blocks_; // 存储关键词块
        std::vector<KeywordItem*> keyword_items_; // 存储关键词项
        std::streampos keyword_blocks_begin_pos_; // 存储关键词块的位置

        // record 相关
         std::vector<RecordBlockInfo*> record_blocks_;
         //std::streampos record_section_begin_pos_; // 存储记录的位置
         std::streampos record_block_begin_pos_; // 存储记录的位置

        uint64_t com_record_blocks_size_; // 压缩后的记录块的大小
        uint64_t decomp_record_blocks_size_; // 解压后的记录块的大小
    };
}