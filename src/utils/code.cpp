#include "include/utils/code.h"

// 注意：由于这些函数在头文件中已经使用inline关键字定义，
// 这里的cpp文件主要是为了完整性，实际使用时编译器会直接内联这些函数
// 如果将来需要添加非内联函数，可以在这里实现

namespace FreeMDict
{
    namespace Utils
    {
        int getCodeTypeSize(CodeType code_type)
        {
            switch (code_type)
            {
            case CodeType::UTF8:
                return 1;
            case CodeType::GBK:
                return 2;
            case CodeType::BIG5:
                return 2;
            case CodeType::UTF16LE:
                return 2;
            case CodeType::UTF16BE:
                return 2;
            default:
                return 0;
            }
        }
        
        uint64_t fromBigEndianUInt64(uint64_t bigEndianValue)
        {
            // 如果本机是大端，直接返回
            if (isBigEndian())
            {
                return bigEndianValue;
            }
            
            return ((bigEndianValue & 0x00000000000000FFULL) << 56) |
           ((bigEndianValue & 0x000000000000FF00ULL) << 40) |
           ((bigEndianValue & 0x0000000000FF0000ULL) << 24) |
           ((bigEndianValue & 0x00000000FF000000ULL) << 8)  |
           ((bigEndianValue & 0x000000FF00000000ULL) >> 8)  |
           ((bigEndianValue & 0x0000FF0000000000ULL) >> 24) |
           ((bigEndianValue & 0x00FF000000000000ULL) >> 40) |
           ((bigEndianValue & 0xFF00000000000000ULL) >> 56);
        }
    
    }
}