// internal
#include "Utils.h"

namespace Isonia::Utils
{
    extern int strcmp(const char* str1, const char* str2)
    {
        while (*str1 && (*str1 == *str2))
        {
            str1++;
            str2++;
        }
        return *(unsigned char*)str1 - *(unsigned char*)str2;
    }

    extern void* memcpy(void* dst, const void* src, unsigned long long len)
    {
        unsigned long long i;
        if ((unsigned long long)dst % sizeof(long) == 0 &&
            (unsigned long long)src % sizeof(long) == 0 &&
            len % sizeof(long) == 0) {

            long* d = reinterpret_cast<long*>(dst);
            const long* s = reinterpret_cast<const long*>(src);

            for (i = 0; i < len / sizeof(long); i++) {
                d[i] = s[i];
            }
        }
        else {
            char* d = reinterpret_cast<char*>(dst);
            const char* s = reinterpret_cast<const char*>(src);

            for (i = 0; i < len; i++) {
                d[i] = s[i];
            }
        }

        return dst;
    }
}
