#include <iostream>
#include <fstream>
#include <thread>
#include <string>

#include "include/base/mdict.h"
#include "include/extern/mdict_extern.h"

#include <gflags/gflags.h>

void listKeywords(const FreeMDict::Mdict &mdict)
{
    const std::vector<FreeMDict::KeywordItem *> &keyword_items = mdict.getKeywordItems();
    for (const FreeMDict::KeywordItem *item : keyword_items)
    {
        std::cout << item->keyword << std::endl;
    }
}

void searchWords(FreeMDict::Mdict &mdict)
{
    while (true)
    {
        std::string word;
        std::cout << "Please enter the word to query: ";
        std::cin >> word;
        if (word == "\\q") {
            break;
        }
        std::string result = mdict.lookup(word);
        std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        std::cout << result;

        std::cout << std::endl << "<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;

    }
}

// Define command line parameters
//DEFINE_string(dict, "", "字典文件路径");
DEFINE_bool(verbose, false, "启用详细输出信息");
DEFINE_bool(keys, false, "列出所有关键词");
DEFINE_bool(search, false, "搜索关键词");
DEFINE_bool(check, false, "检查字典文件");

int main(int argc, char *argv[])
{
    // 解析命令行参数
    gflags::ParseCommandLineFlags(&argc, &argv, true);
        
    // 显示帮助信息
    if (argc == 1) {
        std::cout << "Usage: " << argv[0] << "[options] <dict_path>" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --verbose           Enable verbose output" << std::endl;
        std::cout << "  --keys     List all keywords" << std::endl;
        std::cout << "  --help              Display this help message" << std::endl;
        std::cout << "  <dict_path>       Dictionary file path (required)" << std::endl;
        return 1;
    }

    std::string dict_path (argv[argc-1]);
    int64_t begintime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    FreeMDict::Mdict mdict(dict_path);
    mdict.init();
    // mdict.dumpAll();
    // return 0;

    int64_t end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "init cost time " << end - begintime << " ms" << std::endl;

    if (FLAGS_keys) {
        listKeywords(mdict);
        return 0;
    }else if (FLAGS_search) {
        searchWords(mdict);
        return 0;
    }
    
    return 0;
}