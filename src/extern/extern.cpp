#include <include/extern/mdict_extern.h>

#ifdef __cplusplus
extern "C" {
#endif



void* create_mdict(const char* dict_path){
    // 创建一个输出流指向/dev/null，与main.cpp中的调用方式保持一致
    FreeMDict::Mdict *mdict = new FreeMDict::Mdict(dict_path);
    if (!mdict->init())
    {
        delete mdict;
        return nullptr;
    }
    
    return mdict;
}

void destroy_mdict(void *mdict){
    if(mdict == nullptr){
        return;
    }
    FreeMDict::Mdict *mdict_ptr = (FreeMDict::Mdict *) mdict;
    delete mdict_ptr;
}

void *get_mdict_info(void *mdict){
    return nullptr;
}



char** get_all_keys(void *mdict, int *key_count) {
    if(mdict == nullptr||key_count == nullptr){
        return nullptr;
    }
    FreeMDict::Mdict *mdict_ptr = (FreeMDict::Mdict *) mdict;
    const auto &items = mdict_ptr->getKeywordItems();
    *key_count = items.size();
    char** keys = (char**)malloc(*key_count * sizeof(char*));
    if(keys == nullptr){
        return nullptr;
    }
    
    for(int i = 0; i < *key_count; i++){
        char *ptr = (char*)malloc(items[i]->keyword.length() + 1);
        ptr[items[i]->keyword.length()] = '\0';
        strncpy(ptr, items[i]->keyword.c_str(), items[i]->keyword.length());
        keys[i] = ptr;
    }

    return keys;
}

void free_all_keys(char **keys, int key_count){
    if(keys == nullptr){
        return;
    }
    for(int i = 0; i < key_count; i++){
        free(keys[i]);
    }
    free(keys);
}


// only for mdx file
const char* mdict_lookup(void *mdict, char *keyword){
    if(mdict == nullptr){
        return nullptr;
    }
    FreeMDict::Mdict *mdict_ptr = (FreeMDict::Mdict *) mdict;
    std::string keyword_str(keyword);
    std::string result = mdict_ptr->lookup(keyword_str);
    // 分配内存并复制字符串内容
    char* result_char = (char*)malloc(result.length() + 1);
    if(result_char != nullptr){
        strncpy(result_char, result.c_str(), result.length());
    }
    result_char[result.length()] = '\0';
    return result_char;
}

const char* mdict_locate(void *mdict, char *keyword, int *len){
    if(mdict == nullptr||len == nullptr||keyword == nullptr){
        return nullptr;
    }
    
    FreeMDict::Mdict *mdict_ptr = (FreeMDict::Mdict *) mdict;

    const char *result_str = mdict_ptr->locate(std::string(keyword), *len);

    return result_str;
}

#ifdef __cplusplus
}
#endif

