#include <stdio.h>

#include<string.h>
#include"FileManager.h"


int main() {

    String* fileName = new_String("/home/ubuntu/test.txt");
    File* file = new_File(fileName,'w');
    String* fileContent = new_String("hello world!");
    Buffer* buffer = file->getBuffer(file);
    buffer->setContent(buffer,fileContent);
    file->write(file);
    file->free(file);


}