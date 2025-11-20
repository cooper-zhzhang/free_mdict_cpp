#include <iostream>
#include <fstream>
#include <thread>
#include <string>

#include "include/base/mdict.h"

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
        std::cout << "请输入要查询的单词: ";
        std::cin >> word;
        if (word == "\\q") {
            break;
        }
        mdict.getResourceByKey(word, std::cout);
    }
}

// 定义命令行参数
DEFINE_string(dict, "", "字典文件路径");
DEFINE_bool(verbose, false, "启用详细输出信息");
DEFINE_bool(list_keywords, false, "列出所有关键词");
DEFINE_string(search, "", "搜索关键词");

int main(int argc, char *argv[])
{
    // 解析命令行参数
    gflags::ParseCommandLineFlags(&argc, &argv, true);
        
    // 显示帮助信息
    if (FLAGS_dict.empty()) {
        std::cout << "Usage: " << argv[0] << " --dict=<dict_path> [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --dict=<path>       字典文件路径（必需）" << std::endl;
        std::cout << "  --verbose           启用详细输出" << std::endl;
        std::cout << "  --list_keywords     列出所有关键词" << std::endl;
        std::cout << "  --help              显示此帮助信息" << std::endl;
        return 1;
    }
    

    int64_t begintime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    std::ofstream null_out("/dev/null");
    FreeMDict::Mdict mdict(FLAGS_dict.c_str(), null_out);
    mdict.init();
    mdict.displayInfo();

    int64_t end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "init cost time " << end - begintime << " ms" << std::endl;

    if (FLAGS_list_keywords) {
        listKeywords(mdict);
        return 0;
    }else if (!FLAGS_search.empty()) {
        searchWords(mdict);
        return 0;
    }
    
    return 0;
}