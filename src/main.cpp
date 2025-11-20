#include <iostream>
#include <fstream>
#include <thread>
#include "include/base/mdict.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << " <dict_path>" << endl;
        return 1;
    }

    // 创建一个输出到/dev/null的流，相当于"空"输出流
    int64_t begintime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::ofstream null_out("/dev/null");
    FreeMDict::Mdict mdict(argv[1], null_out);

    
    mdict.init();
    #ifdef SMALL_MEMORY
    mdict.displayInfo();
    #endif //SMALL_MEMORY
    
    // FreeMDict::Mdict mdict1(argv[1], null_out);
    // mdict1.init();
    
    // FreeMDict::Mdict mdict2(argv[1], null_out);
    // mdict2.init();
    
    // FreeMDict::Mdict mdict3(argv[1], null_out);
    // mdict3.init();
    // FreeMDict::Mdict mdict4(argv[1], null_out);
    // mdict4.init();
    
    int64_t end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "init cost time " << end - begintime << " ms" << std::endl;

    // while (1)
    // {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //     std::string word;
    //     word = "name";
    //     //std::cin >> word; // 清除输入缓冲区中的换行符
    //     if(word.empty())
    //     {
    //         continue;
    //     }
    //     if (word == "exit")
    //     {
    //         break;
    //     }
        
    //     mdict.getResourceByKey(word, cout);
    // }

    const std::vector<FreeMDict::KeywordItem *> &keyword_items = mdict.getKeywordItems();
    for (int i = 0; i < keyword_items.size(); ++i) {
        ofstream mp3_file("1.html", ios::binary);
        if (mp3_file.is_open())
        {
            if(mdict.getResourceByKey(keyword_items[i]->keyword_utf8, mp3_file)){
                mp3_file.close();
            }   else {
                cout << "获取资源失败" << endl;
            }
        }   
        if(i ==  keyword_items.size()-1){
            i = 0;
        }
    }

    // // 打开一个名为1.mp3的文件
    // ofstream mp3_file("1.mp3", ios::binary);
    // if (mp3_file.is_open())
    // {
    //     mdict.getResourceByKey("\\check__gb_2.mp3\0", mp3_file);
    //     mp3_file.close();
    //     cout << "资源已写入1.mp3文件" << endl;
    // }
    // else
    // {
    //     cout << "无法创建文件1.mp3" << endl;
    // }

    return 0;
}