#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <string.h>
#include <ctype.h>
#include "stdio.h"

/*将字符串按指定字符拆分*/
int split(char *src,const char*sep,char **dest);

/*将字符串按指定字符串拆分*/
int split_str(char *src,const char*sep,char **dest);

int strtoInt(char *str);
#endif