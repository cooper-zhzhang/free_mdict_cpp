#include <include/base/mdict.h>

#ifdef __cplusplus
extern "C" {
#endif



void* create_mdict(const char* dict_path);


void destroy_mdict(void *mdict);



char** get_all_keys(void *mdict, int *key_count) ;

void free_all_keys(char **keys, int key_count);

// only for mdx file
const char* mdict_lookup(void *mdict, char *keyword);

const char* mdict_locate(void *mdict, char *keyword, int *len);

#ifdef __cplusplus
}
#endif

