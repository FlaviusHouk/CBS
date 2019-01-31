#include "Helper.h"

gboolean g_string_compare(GString* str1, GString* str2)
{
    if(str1->len != str2->len)
        return FALSE;

    return strncmp(str1->str, str2->str, str1->len);
}

gboolean g_string_starts_with(GString* str, GString* startsWith)
{
    if(startsWith->len > str->len)
        return FALSE;

    return strncmp(str->str, startsWith->str, startsWith->len);
}

/*gint g_string_index_of(const GString* str, const GString* seek)
{
    if(seek->len > str->len)
        return -1;

    gint toRet = -1;

    int i = 0;
    while(i < str->len - seek->len)
    {
        if(strncmp(str->str + i, seek->str, seek->len) == 0)
            toRet = i;

        i++;
    }

    return toRet;
}*/