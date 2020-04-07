/*
 * Creator: Naman Dixit
 * Notice: © Copyright 2020 Naman Dixit
 */

#if !defined(NLIB_STRING_H_INCLUDE_GUARD)

header_function
Sint stringCompare(Char *s1, Char *s2)
{
    while(s1[0]) {
        if (s1[0] != s2[0]) break;
        s1++;
        s2++;
    }

    return s1[0] - s2[0];
}

header_function
Size stringLength(Char *s)
{
    Size len = 0;

    for (Size i = 0; s[i] != '\0'; i++) {
        len++;
    }

    len++;

    return len;
}

header_function
B32 stringEqual (Char *str1, Char *str2)
{
    B32 result = (stringCompare(str1, str2) == 0);
    return result;
}

header_function
Size stringPrefix(Char *str, Char *pre)
{
    Size lenpre = stringLength(pre);
    Size lenstr = stringLength(str);

    if (lenstr < lenpre) {
        return 0;
    } else {
        if (memcmp(pre, str, lenpre) == 0) {
            return lenpre;
        } else {
            return 0;
        }
    }
}

header_function
Size stringSuffix (Char *str, Char *suf)
{
    Size lensuf = stringLength(suf);
    Size lenstr = stringLength(str);

    if (lenstr < lensuf) {
        return 0;
    } else {
        if (memcmp(suf, str + (lenstr - lensuf), lensuf) == 0) {
            return lensuf;
        } else {
            return 0;
        }
    }
}

#define NLIB_STRING_H_INCLUDE_GUARD
#endif