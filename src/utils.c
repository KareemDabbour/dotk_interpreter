#include "include/utils.h"
#include "stdlib.h"

int fast_compare(const char *ptr0, const char *ptr1, int len)
{
    int fast = len / sizeof(size_t) + 1;
    int offset = (fast - 1) * sizeof(size_t);
    int current_block = 0;

    if (len <= sizeof(size_t))
    {
        fast = 0;
    }

    size_t *lptr0 = (size_t *)ptr0;
    size_t *lptr1 = (size_t *)ptr1;

    while (current_block < fast)
    {
        if ((lptr0[current_block] ^ lptr1[current_block]))
        {
            int pos;
            for (pos = current_block * sizeof(size_t); pos < len; ++pos)
            {
                if ((ptr0[pos] ^ ptr1[pos]) || (ptr0[pos] == 0) || (ptr1[pos] == 0))
                {
                    return (int)((unsigned char)ptr0[pos] - (unsigned char)ptr1[pos]);
                }
            }
        }

        ++current_block;
    }

    while (len > offset)
    {
        if ((ptr0[offset] ^ ptr1[offset]))
        {
            return (int)((unsigned char)ptr0[offset] - (unsigned char)ptr1[offset]);
        }
        ++offset;
    }

    return 0;
}