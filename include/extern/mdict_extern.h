#include <include/base/mdict.h>

#ifdef __cplusplus
extern "C" {
#endif



void* get_mdict(const char* dict_path){
    //return new FreeMDict::Mdict(dict_path);
    return nullptr;
}

void *get_mdict_info(void *mdict){
    return nullptr;
}

int mdict_search(void *mdict){
    if(mdict == nullptr){
        return -1;
    }
    FreeMDict::Mdict *mdict_ptr = (FreeMDict::Mdict *) mdict;
    mdict_ptr->search("hello");
    return 0;
}

#ifdef __cplusplus
}
#endif

