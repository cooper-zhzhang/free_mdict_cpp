#include <iostream>
#include <string>

#include "include/base/mdict.h"
#include "include/extern/mdict_extern.h"


#include <gflags/gflags.h>


void checkCFunc(const std::string &dict_path){

    void *mdict = create_mdict(dict_path.c_str());
    if(mdict == nullptr){
        std::cout << "get_mdict_info failed" << std::endl;
    }else {
        std::cout << "get_mdict_info success" << std::endl;
    }

    int key_count = 0;
    char **keys = get_all_keys(mdict, &key_count);
    if(keys == nullptr){
        std::cout << "get_all_keys failed" << std::endl;
    }else {
        std::cout << "get_all_keys success" << std::endl;
    }
    for(int i = 0; i < key_count; i++){
        std::cout << keys[i] << std::endl;
    }

   free_all_keys(keys, key_count);

   int len;
   const char* result = mdict_locate(mdict, "\\zygote__us_1.mp3", &len);
   if(result == nullptr){
        std::cout << "mdict_locate failed" << std::endl;
   }else {
        std::cout << "mdict_locate success," << "len: " << len << std::endl;
        for(int i = 0; i < len; i++){
            std::cout << int(result[i]) << " ";
        }
        std::cout << std::endl;
   }


    destroy_mdict(mdict);
}