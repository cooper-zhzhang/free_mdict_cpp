#pragma once

#include <zlib.h>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <map>
#include <string>
#include <cstring>

#include <codecvt>
#include <locale>
namespace FreeMDict
{   
    namespace Utils
    {
        u_char* zlibDecompress(u_char *key_index_comp, uint64_t key_index_comp_len, uint64_t key_index_decomp_len)
        {
            u_char *decomp = new u_char[key_index_decomp_len];

            // Use zlib for decompression
            z_stream zs;
            memset(&zs, 0, sizeof(zs));

            // Initialize zlib decompression stream
            if (inflateInit(&zs) != Z_OK)
            {
                delete[] decomp;
                return nullptr;
            }

            // Set input buffer
            zs.next_in = reinterpret_cast<Bytef *>(key_index_comp);
            zs.avail_in = static_cast<uInt>(key_index_comp_len);

            // Set output buffer
            zs.next_out = reinterpret_cast<Bytef *>(decomp);
            zs.avail_out = static_cast<uInt>(key_index_decomp_len);

            // Execute decompression
            int ret = inflate(&zs, Z_FINISH);

            // Clean up zlib stream
            inflateEnd(&zs);

            // Check if decompression succeeded
            if (ret != Z_STREAM_END)
            {
                delete[] decomp;
                return nullptr;
            }

            return decomp;
        }
 
        const uint32_t NO_COMP_TYPE = 0x00 << 24 | 0x00 << 16 | 0x00 << 8 | 0x00;
        const uint32_t LZO_COMP_TYPE = 0x00 << 24 | 0x00 << 16 | 0x00 << 8 | 0x01;
        const uint32_t ZLIB_COMP_TYPE = 0x00 << 24 | 0x00 << 16 | 0x00 << 8 | 0x02;
    }
}