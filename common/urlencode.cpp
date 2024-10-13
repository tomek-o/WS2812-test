#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "urlencode.h"

char* urlencode(char* buf, size_t blen, const char* s)
{
    unsigned int i, j = 0;
    for (i=0; s[i]!='\0' && j+1<blen; ++i) {
        if ((s[i]>='A'&&s[i]<='Z') ||
            (s[i]>='a'&&s[i]<='z') ||
            (s[i]>='0'&&s[i]<='9') ||
            NULL!=strchr("-_.!~*'();/?:@&=+$,#", s[i]))
            buf[j++] = s[i];
        else {
            if (j+4<=blen) {
                snprintf(buf+j, 4, "%%%02X", (unsigned char)s[i]);
                j += 3;
            }
            else
                break;
        }
    }
    buf[j] = '\0';
    return buf;
}

/* url encode */
char* urlencode_all(char* buf, size_t blen, const char* s)
{
    unsigned int i, j = 0;
    for (i=0; s[i]!='\0' && j+1<blen; ++i) {
        if ((s[i]>='A'&&s[i]<='Z') ||
            (s[i]>='a'&&s[i]<='z') ||
            (s[i]>='0'&&s[i]<='9') ||
            NULL!=strchr("-_.", s[i]))
            buf[j++] = s[i];
        else {
            if (j+4<=blen) {
                snprintf(buf+j, 4, "%%%02X", (unsigned char)s[i]);
                j += 3;
            }
            else
                break;
        }
    }
    buf[j] = '\0';
    return buf;
}

/* hex char(eg. 'e','f') to int. */
static int chex2int(char h)
{
    if (h>='0' && h<='9') return h-'0';
    if (h>='a' && h<='f') return h-'a'+10;
    if (h>='A' && h<='F') return h-'A'+10;
    return 0;
}

/* url decode */
char* urldecode(char* buf, size_t blen, const char* s)
{
    unsigned int i, j = 0;
    for (i=0; i+1<blen && s[j]!='\0'; ++i) {
        if (s[j]=='%') {
            if (s[j+1]!='\0' && s[j+2]!='\0') {
                buf[i] = (char)(chex2int(s[j+1])*16+chex2int(s[j+2]));
                j += 3;
                continue;
            }
        }
        buf[i] = s[j++];
    }
    buf[i] = '\0';
    return buf;
}
