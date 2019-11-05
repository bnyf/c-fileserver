//
// Created by lwx on 11/2/19.
//

#ifndef FILEUPANDDOWN_UTILITY_H
#define FILEUPANDDOWN_UTILITY_H


#include <stdint.h>

typedef struct AutoAllocate{

    char* (*autoAllocate)(uint32_t baseSize,uint32_t nums,const char* oldContent,uint32_t size);
    char* (*multiple_autoAllocate)(uint32_t multiple_times,const char* oldContent,uint32_t size);
    char* (*linear_autoAllocate)(int32_t a,int32_t b,const char* oldContent,uint32_t size);

}AutoAllocator;
AutoAllocator* new_AutoAllocate();

typedef struct String{

    char* content;
    __uint32_t size;
    __uint32_t (*getSize)(const struct String* string);
    char* (*getContent)(const struct String* string);

} String;
String* new_String(char* content);


#endif //FILEUPANDDOWN_UTILITY_H
