#ifndef _HTTPSERVICE_
#define _HTTPSERVICE_

#include "instance.h"
#include <curl/curl.h>
#include "cJSON.h"

typedef enum {
    HttpContentType_ApplicationJson,
    HttpContentType_ApplicationXml,
    HttpContentType_ApplicationUrlEncoded,
    HttpContentType_TextPlain,
    HttpContentType_TextXml,
} HttpContentType;

typedef struct HttpService {
    Instance instance;

    CURL *curl;
} HttpService;

HttpService *HttpService_new(const char *className, Instance *parent);

const char *HttpService_GetAsync(HttpService *this, const char *url);
const char *HttpService_PostAsync(HttpService *this, const char *url, const char *data, HttpContentType contentType);

cJSON *HttpService_JSONDecode(HttpService *this, const char *input);
const char *HttpService_JSONEncode(HttpService *this, cJSON *input);

void serialize_HttpService(HttpService *httpservice, SerializeInstance *inst);

#endif
