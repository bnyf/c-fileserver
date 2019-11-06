//
// Created by lwx on 11/2/19.
//

#include "FileManager.h"
#include<stdlib.h>


/***
 * File class area
 * @param file
 * @return
 */

static int32_t File_write(const File* file){

    Buffer* buffer = file->buffer;
    int32_t status = fputs(buffer->content,file->fp);
    if(status == EOF){

        return WRITE_ERROR;
    }
    return WRITE_RIGHT;

}
static void File_readAll(File* file){

    Buffer* buffer = file->buffer;
    char* status = fgets(buffer->content,buffer->maxSize,file->fp);

}

static void File_read(File* file,uint32_t size){

    Buffer* buffer = file->buffer;

    if(size > buffer->maxSize){

        size = buffer->maxSize;
    }

    char* status = fgets(buffer->content,size,file->fp);

}

static void File_free(File* file){

    file->buffer->free(file->buffer);
    file->close(file);
    free(file->fp);
    free(file);

}
static void File_open(File* file){

    if(file->fp != NULL){

        file->close(file);
    }
    file->fp = fopen(file->fileName->content,&file->mode);


}
static void File_close(File* file){

    fclose(file->fp);

}
static String* File_getFileName(const File* file){

    return file->fileName;

}
static Buffer* File_getBuffer(const File* file){

    return file->buffer;
}
static void File_setCursorToStart(File* file){

    file->setCursor(file,START,0);
}
static void File__setCursorToLast(File* file){

    file->setCursor(file,LAST,0);

}
static void File_setCursor(struct File* file,FileAbsolutePosition absPosition,int64_t relativePosition){

    fseek(file->fp,relativePosition,absPosition);

}
static void File_readFromCustomIndex(struct File* file,uint32_t size,FileAbsolutePosition absPosition,int64_t relativePosition){

    file->setCursor(file,absPosition,relativePosition);
    file->read(file,size);
}
static void readAllFromCustomIndex(struct File* file,FileAbsolutePosition absPosition,int64_t relativePosition){

    file->setCursor(file,absPosition,relativePosition);
    file->readAll(file);

}
File* new_File(const String* fileName,const char mode){

    if(mode != 'r' &&  mode != 'w'){

        return 0;
    }
    File* file = malloc(sizeof(File));
    FILE* p = fopen(fileName->content,&mode);
    file->fp = p;
    file->mode = mode;
    file->buffer = new_Buffer(0);
    file->write = File_write;
    file->read = File_read;
    file->readAll = File_readAll;
    file->free = File_free;
    file->open = File_open;
    file->close = File_close;
    file->getFileName = File_getFileName;
    file->getBuffer = File_getBuffer;
    file->setCursorToStart = File_setCursorToStart;
    file->setCursorToLast = File__setCursorToLast;
    file->setCursor = File_setCursor;
    file->readFromCustomIndex = File_readFromCustomIndex;
    file->readAllFromCustomIndex = readAllFromCustomIndex;

    return file;

}


/***
 *
 * Buffer class area
 *
 */

__uint32_t BUFFER_SIZE = 0x00080000;
static uint32_t Buffer_getMaxSize(const Buffer* buffer){

    return buffer->maxSize;
}
static uint32_t Buffer_getCurrentLength(const Buffer* buffer){

    return buffer->currentLength;
}
static char* Buffer_getContent(const Buffer* buffer){

    return buffer->content;

}
static void Buffer_setContent(Buffer* buffer,const String* string){

    for(uint32_t i = 0; i < string->getSize(string); ++i){

        if(i >= buffer->maxSize){

            break;
        }
        buffer->content[i] = string->content[i];
    }


}
static void Buffer_setCurrentLength(Buffer* buffer,uint32_t currentLength){

    buffer->currentLength = currentLength;

}
static void Buffer_autoSetCurrentLength(Buffer* buffer){

    int i = 0;
    for(;buffer->content[i] != 0; ++i);

    buffer->currentLength = i;

}
static void Buffer_free(Buffer* buffer){

    free(buffer->content);

}

Buffer* new_Buffer(uint32_t bufferSize){

    Buffer* buffer = malloc(sizeof(Buffer));

    if(bufferSize <= 0){

        buffer->maxSize = BUFFER_SIZE;
    }
    else{

        buffer->maxSize = bufferSize;
    }
    buffer->currentLength = 0;
    buffer->content = malloc(buffer->maxSize*sizeof(char));

    buffer->getMaxSize = Buffer_getMaxSize;
    buffer->getCurrentLength = Buffer_getCurrentLength;
    buffer->getContent = Buffer_getContent;
    buffer->setContent = Buffer_setContent;
    buffer->setCurrentLength = Buffer_setCurrentLength;
    buffer->autoSetCurrentLength = Buffer_autoSetCurrentLength;
    buffer->free = Buffer_free;


    return buffer;

}

