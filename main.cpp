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


void py2gb(int id, char* ims, char* buf)
{
    char fds[80];
    FILE *fp;
    char* ptr;
    int max, pos, num, len;
    len = strlen(ims);
    if (id < 0 || len <= 0 || len > 6)
    {
        return;
    }


    // 独立拼音
    strcpy(fds, "a   e   m   n   o   ai  an  ao  ei  en  er  hm  ng  ou  ang eng hng");
    ptr = strstr(fds, ims);
    if (ptr != 0 && ((ptr - fds)&3) == 0)
    {
        pos = (ptr - fds) + 282;
    }
    else
    {
        // 声母
        strcpy(fds, "hbchdfgjklmnpqrshtwxyzh");
        if (!(ptr = strchr(fds,  *ims)))
        {// 声母不存在
            return;
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
            return;
        }
        pos = pos + (ptr - fds);
    }
    fp = fopen("py2gb.ime", "rb");
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
    }

    fclose(fp);


}

int main() {
    char hz[32];
    char ims[]="li";
    py2gb(10, ims, hz);
    hz[2]=0;
    char dst_utf8[1024] = {0};
    GbkToUtf8(hz, strlen(hz), dst_utf8, sizeof(dst_utf8));
    std::cout <<dst_utf8 << std::endl;
    return 0;
}
