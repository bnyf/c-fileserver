//
// Created by ubuntu on 11/2/19.
//

#ifndef FILEUPANDDOWN_FILEMANAGER_H
#define FILEUPANDDOWN_FILEMANAGER_H

#include <stdio.h>
#include<stdint.h>
#include "utility.h"

enum FileOperateState{

    WRITE_ERROR,
    WRITE_RIGHT
};
typedef struct Buffer{

    __uint32_t maxSize;
    uint32_t  currentLength;
    char* content;
    uint32_t (*getMaxSize)(const struct Buffer* buffer);
    uint32_t (*getCurrentLength)(const struct Buffer* buffer);
    char* (*getContent)(const struct Buffer* buffer);
    void (*setContent)(struct Buffer* buffer,const String* string);
    void (*setCurrentLength)(struct Buffer* buffer,uint32_t currentLength);
    void (*autoSetCurrentLength)(struct Buffer* buffer);
    void (*free)(struct Buffer* buffer);


}Buffer;
Buffer* new_Buffer(uint32_t bufferSize);


typedef enum FileAbsolutePosition{

    START,
    CURRENT,
    LAST
}FileAbsolutePosition;
typedef struct File{

    String* fileName;
    FILE* fp;
    char mode;
    Buffer* buffer;
    int32_t (*write)(const struct File* file);
    void (*read)(struct File* file,uint32_t size);
    void (*readAll)(struct File* file);
    void (*free)(struct File* file);
    void (*open)(struct File* file);
    void (*close)(struct File* file);
    String* (*getFileName)(const struct File* file);
    Buffer* (*getBuffer)(const struct File* file);
    void (*setCursorToStart)(struct File* file);
    void (*setCursorToLast)(struct File* file);
    void (*setCursor)(struct File* file,FileAbsolutePosition absPosition,int64_t relativePosition);
    void (*readFromCustomIndex)(struct File* file,uint32_t size,FileAbsolutePosition absPosition,int64_t relativePosition);
    void (*readAllFromCustomIndex)(struct File* file,FileAbsolutePosition absPosition,int64_t relativePosition);

}File;

File* new_File(const String* fileName,const char mode);



//typedef struct FileManager{
//
//    Buffer* buffer;
//
//    File* (*open)(const String* fileName,const char mode);
//    int32_t (*close)(const File* file);
//    String (*downloadFile)(const String* fileName);
//    void (*uploadFile)(const String* fileName,const String* content);
//    void (*free)(struct FileManager* fileManager);
//
//
//}FileManager;




#endif //FILEUPANDDOWN_FILEMANAGER_H
