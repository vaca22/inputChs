#include <iostream>
#include <cstring>



#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>

using namespace std;


#include <iconv.h>

int GbkToUtf8(char *str_str, size_t src_len, char *dst_str, size_t dst_len)
{
    iconv_t cd;
    char **pin = &str_str;
    char **pout = &dst_str;

    cd = iconv_open("utf8", "gbk");
    if (cd == 0)
        return -1;
    memset(dst_str, 0, dst_len);
    if (iconv(cd, pin, &src_len, pout, &dst_len) == -1)
        return -1;
    iconv_close(cd);
    *pout = 0;

    return 0;
}

int Utf8ToGbk(char *src_str, size_t src_len, char *dst_str, size_t dst_len)
{
    iconv_t cd;
    char **pin = &src_str;
    char **pout = &dst_str;

    cd = iconv_open("gbk", "utf8");
    if (cd == 0)
        return -1;
    memset(dst_str, 0, dst_len);
    if (iconv(cd, pin, &src_len, pout, &dst_len) == -1)
        return -1;
    iconv_close(cd);
    *pout = 0;

    return 0;
}
























char lava_GetWordMode = 0;
FILE *IME_fp=NULL;


























int qw2gb(char* qwc, char* buf)
{
    char gb[4];
    if (strlen(qwc) != 4)
    {
        return  -1;
    }
    gb[0] = *(qwc + 0) *10+*(qwc + 1) - 368;
    gb[1] = *(qwc + 2) *10+*(qwc + 3) - 368;
    gb[2] = 0;
    if (gb[0] > 160 && gb[1] > 160)
    {
        strcpy(buf, gb);
    }
    else
    {
        return  -1;
    }
    return gb[1] << 8 | gb[0];
}

long py2gb(int id, char* ims, char* buf)
{
    char fds[80];
    FILE *fp;
    char* ptr;
    int max, pos, num, len, m;
    len = strlen(ims);
    if (id < 0 || len <= 0 || len > 6)
    {
        return  -1;
    }


    if (*ims == 'u')
    {
        if (len == 2)
        {
            num = *(ims + 1);
        }
        else
        if (len == 3 && *(ims + 1) == 'u')
        {
            num = *(ims + 2);
        }
        else
        {
            return  -1;
        }
        if (num >= 'a' && num <= 'z')
        {
            *buf = 163;
            *(buf + 1) = len * 32 + 32 + num;
        }
        else
        if (num >= '1' && num <= '9')
        {
            *buf = 162;
            *(buf + 1) = len * 52 + 24 + num;
        }
        else
        {
            return -1;
        }
        *(buf + 2) = 0;
        return 0x00010001;
    }


    if (*ims == 'i')
    {
        if (len == 2)
        {
            num = *(ims + 1);
        }
        else
        if (len == 3 && *(ims + 1) == 'i')
        {
            num = *(ims + 2);
        }
        else
        {
            return  -1;
        }
        if (num >= 'a' && num <= 'x')
        {
            *buf = 166;
            *(buf + 1) = len * 32 + num;
        }
        else
        {
            return -1;
        }
        *(buf + 2) = 0;
        return 0x00010001;
    }

    // 独立拼音
    strcpy(fds, "a   e   m   n   o   ai  an  ao  ei  en  er  hm  ng  ou  ang eng hng");
    ptr = strstr(fds, ims);
    if (ptr > 0 && ((ptr - fds)&3) == 0)
    {
        pos = (ptr - fds) + 282;
    }
    else
    {
        // 声母
        strcpy(fds, "hbchdfgjklmnpqrshtwxyzh");
        if (!(ptr = strchr(fds,  *ims)))
        {// 声母不存在
            return  -1;
        }
        //ch sh zh
        if (*++ims == 'h')
        {
            ims++;
            ptr++;
            len--;
        }
        pos = (ptr - fds) * 132 + 350;
        // 韵母
        if (len == 2)
        {
            strcpy(fds, "a   e   i   o   u   v");
        }
        else
        if (len == 3)
        {
            strcpy(fds, "ai  an  ao  ei  en  ia  ie  in  iu  ou  ua  ue  ui  un  uo  ve");
            pos = pos + 24;
        }
        else
        if (len >= 4)
        {
            strcpy(fds, "ang eng ian iao ing ong uai uan iangionguang");
            pos = pos + 88;
        }
        if (!(ptr = strstr(fds, ims)))
        {// 韵母不存在
            return  -1;
        }
        pos = pos + (ptr - fds);
    }
    if ((fp=IME_fp) == NULL)
    {
        if ((fp = fopen("py2gb.ime", "rb")) == NULL)
        {
            return  -1;
        }
    }
    fseek(fp, pos, 0);
    max = getc(fp) | getc(fp)<<8;
    if (max == -1 || max <= id)
    {
        num = -1;
    }
    else
    {
        pos = getc(fp) | getc(fp)<<8;
        if ((num = max - id) > 9)
        {
            num = 9;
        }
        fseek(fp, (id << 1) + pos, 0);
        fread(buf, 1, num << 1, fp);
        *(buf + (num << 1)) = 0;
    }
    if (IME_fp == NULL)
    {
        fclose(fp);
    }
    return (long)max << 16 | num;
}

int main() {
    char hz[32];
    char ims[]="a";
    ims[1]=0;
    long res = py2gb(0, ims, hz);
    hz[2]=0;
    char dst_gbk[1024] = {0};
    char dst_utf8[1024] = {0};
    GbkToUtf8(hz, strlen(hz), dst_utf8, sizeof(dst_utf8));
    std::cout <<dst_utf8 << std::endl;


    return 0;
}
