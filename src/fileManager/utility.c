//
// Created by lwx on 11/2/19.
//

#include "utility.h"
#include<stdlib.h>


static void moveOldContentToNew(char* newContent,const char* oldContent,uint32_t size){

    for(uint32_t i = 0; i < size;++i){

        newContent[i] = oldContent[i];
    }

}
static char* AutoAllocator_autoAllocate(uint32_t baseSize,uint32_t nums,const char* oldContent,uint32_t size){

    char* newContent = malloc(baseSize*nums);
    moveOldContentToNew(newContent,oldContent,size);
    return newContent;
}

static char* AutoAllocator_multiple_autoAllocate(uint32_t multiple_times,const char* oldContent,uint32_t size){

    char* newContent = malloc(multiple_times*size);
    moveOldContentToNew(newContent,oldContent,size);
    return newContent;
}
static char* AutoAllocator_linear_autoAllocate(int32_t a,int32_t b,const char* oldContent,uint32_t size){

    char* newContent = malloc(a*size+b);
    moveOldContentToNew(newContent,oldContent,size);
    return newContent;
}
AutoAllocator* new_AutoAllocate(){

    AutoAllocator* autoAllocator = malloc(sizeof(AutoAllocator));
    autoAllocator->autoAllocate = AutoAllocator_autoAllocate;
    autoAllocator->multiple_autoAllocate = AutoAllocator_multiple_autoAllocate;
    autoAllocator->linear_autoAllocate = AutoAllocator_linear_autoAllocate;

    return autoAllocator;

}



/***
 *
 * String class area
 *
 */


static void new_content(String* string,const char* content){


    u_int32_t size = string->size;
    string->content = malloc((size+1)* sizeof(char));

    uint32_t i = 0;
    for(;content[i] != 0;++i){

        string->content[i] = content[i];
    }
    string->size = i;

}

static __uint32_t getSize(const String* string){

    return string->size;
}
static char* getContent(const String* string){

    return string->content;

}

String* new_String(char* content){


    String* string = malloc(sizeof(String));

    string->getSize = getSize;
    string->getContent = getContent;
    new_content(string,content);

    return string;

}
