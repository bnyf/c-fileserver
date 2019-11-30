//
// Created by ubuntu on 11/29/19.
//
#include "file.h"
#include "string.h"
#include "stdint.h"
const char* CUSTOMED_PATH = "./content/";

FILE* openFileWithCustomedPath(char* path,char* mode){

    if(path == 0){

        return 0;
    }
    int res = strcmp(path,"/");
    if(res == 0){

        return 0;
    }
    uint32_t length = strlen(path);
    char arr[length + strlen(CUSTOMED_PATH) + 1];
    arr[0] = 0;
    strcat(arr,CUSTOMED_PATH);
    strcat(arr,path);
    return fopen(arr,mode);

}
#include "file.h"
