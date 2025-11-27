#include "include/utils/code.h"

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