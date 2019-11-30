#include <stdlib.h>
#include "tools.h"

/**
 * name: split
 * desc: 将字符串按指定字符进行切分
 * para:
 *     @src     源字符串
 *     @sep     分割字符（可以是多个字符，但按字符拆分）
 *     @dest    指向字串的字符指针
 * return:切分字串的数量
*/
int split(char *src, const char *separator, char **dest)
{
   char *pNext;
   int count = 0;
   if (src == NULL || strlen(src) == 0)
      return 0;
   if (separator == NULL || strlen(separator) == 0)
      return 0;
   pNext = strtok(src, separator);
   while (pNext != NULL)
   {
      *dest++ = pNext;
      ++count;
      pNext = strtok(NULL, separator);
   }
   return count;
}

/**
 * name: split_str
 * desc: 将字符串按指定字符串进行切分
 * para:
 *     @src     源字符串
 *     @sep     分割字符串
 *     @dest    指向字串的字符指针
 * return:切分字串的数量
*/
int split_str(char *src, const char *separator, char **dest)
{
   char *pNext;
   int count = 1;
   *dest++=src;
   if (src == NULL || strlen(src) == 0)
      return 0;
   if (separator == NULL || strlen(separator) == 0)
      return 0;
   int len = strlen(separator);
   pNext = strstr(src, separator);
   while (pNext != 0)
   {
      *pNext='\0';
      *dest++ = pNext+len;
      ++count;
      pNext = strstr(pNext+len, separator);
   }
   return count;
}

int strtoInt(char *str)
{
    return atoi(str);
}