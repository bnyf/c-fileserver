//
// Created by lwx on 11/8/19.
//

#ifndef FILEUPANDDOWN_RESPONSE_H
#define FILEUPANDDOWN_RESPONSE_H

#include "stdint.h"
#include "stdlib.h"

extern const uint32_t CHUNKED_PART_SIZE;

extern const uint32_t RESPONSE_STATUS_LINE_STR_SIZE;
extern const uint32_t RESPONSE_HEADER_STR_SIZE;

extern const char* HTTP_VERSION1_1;
extern const char* HTTP_VERSION1_0;
extern const char* HTTP_VERSION2_0;

extern const uint32_t BAD_REQUEST_CODE;
extern const char* BAD_REQUEST_WORD;

extern const uint32_t NOT_FOUND_CODE;
extern const char* NOT_FOUND_WORD;

extern const uint32_t OK_CODE ;
extern const char* OK_WORD;

extern const uint32_t INTERNAL_SERVER_ERROR_CODE;
extern const char* INTERNAL_SERVER_ERROR_WORD;

extern const char* CONTENT_LENGTH_KEY;

extern const char* CONTENT_TYPE_KEY ;
extern const char* CONTENT_TYPE_VALUE_FILE;

extern const char* ACCEPT_RANGES_KEY ;
extern const char* ACCEPT_RANGES_VALUES_BYTES;

extern const char* TRANSFER_ENCODING_KEY;
extern const char* TRANSFER_ENCODING_VALUE_CHUNKED;

extern const char* CONNECTION_KEY;

typedef struct ResponseStatusLine{

    const char* version;
    uint32_t statusCode;
    const char*  statusWord;

}ResponseStatusLine;
void init_ResponseStatusLine(ResponseStatusLine* responseStatusLine,const char* version,uint32_t statusCode);
ResponseStatusLine* new_ResponseStatusLine(const char* version,uint32_t statusCode);
char* generateStatusLineStr(ResponseStatusLine* responseStatusLine);
const char* getStatusWord(uint32_t statusCode);
void free_ResponseStatusLine(ResponseStatusLine* responseStatusLine);

typedef struct ResponseHeader{

    const char* Content_Type;
    uint32_t Content_Length;
    const char* Accept_Ranges;
    const char* Transfer_Encoding;
    const char* Connection;

}ResponseHeader;
void init_ResponseHeader(ResponseHeader* responseHeader,const char* Content_Type,uint32_t Content_Length,const char* Accept_Ranges,const char* Transfer_Encoding,const char* Connection);
ResponseHeader* new_ResponseHeader(const char* Content_Type,uint32_t Content_Length,const char* Accept_Ranges,const char* Connection);
ResponseHeader* new_ResponseHeader1(const char* Content_Type,const char* Transfer_Encoding,const char* Connection);
ResponseHeader* new_onErrorHeader(const char* Connection);
char* generateResponseHeaderStr(ResponseHeader* responseHeader);
void free_ResponseHeader(ResponseHeader* responseHeader);

typedef struct ResponseBody{

    char* content;
    uint32_t contentLength;

}ResponseBody;
void init_ResponseBody(ResponseBody* responseBody,char* content,uint32_t contentLength);
ResponseBody* new_ResponseBody(char* content,uint32_t contentLength);
char* generateResponseBodyStr(ResponseBody* responseBody);
void free_ResponseBody(ResponseBody* responseBody);

typedef struct Response{

    ResponseStatusLine* statusLine;
    ResponseHeader* responseHeader;
    ResponseBody* responseBody;

}Response;
void init_Response(Response* response,ResponseStatusLine* statusLine,ResponseHeader* responseHeader,ResponseBody* responseBody);
Response* new_Response(ResponseStatusLine* statusLine,ResponseHeader* responseHeader,ResponseBody* responseBody);
char* generateResponseStr(Response* response,uint32_t* responseLength);
void free_Response(Response* response);

#endif //FILEUPANDDOWN_RESPONSE_H

