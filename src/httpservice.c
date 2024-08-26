#include "httpservice.h"
#include <string.h>
#include <raylib.h>

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(httpservice)

HttpService *HttpService_new(const char *className, Instance *parent)
{
    HttpService *newInst = Instance_new("HttpService", parent);

    newInst->instance.DataCost = sizeof(HttpService);
    newInst = realloc(newInst, sizeof(HttpService));

    curl_global_init(CURL_GLOBAL_ALL);

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

struct MemoryStruct {
  char *memory;
  size_t size;
};
 
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

const char *HttpService_GetAsync(HttpService *this, const char *url, int *dataSize)
{
    struct MemoryStruct chunk = { malloc(1), 0 };

    this->curl = curl_easy_init();

    curl_easy_setopt(this->curl, CURLOPT_URL, url);
    curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &chunk);
    curl_easy_setopt(this->curl, CURLOPT_ACCEPT_ENCODING, "deflate"); // this results in virtually uncompressed data because no server likes deflate

    CURLcode res = curl_easy_perform(this->curl);

    if (res != CURLE_OK)
    {
        printf("Error: %s\n", curl_easy_strerror(res));
        return NULL;
    }

    curl_easy_cleanup(this->curl);

    if (dataSize) *dataSize = chunk.size;

    return chunk.memory;
}

const char *HttpService_PostAsync(HttpService *this, const char *url, const char *data, HttpContentType contentType)
{
    FIXME("this %p, url %s, data %s, contentType %d stub\n", this, url, data, contentType);
    return NULL;
}

cJSON *HttpService_JSONDecode(HttpService *this, const char *input)
{
    return cJSON_Parse(input);
}

const char *HttpService_JSONEncode(HttpService *this, cJSON *input)
{
    return cJSON_Print(input);
}

void serialize_HttpService(HttpService *httpservice, SerializeInstance *inst)
{
    serialize_Instance(httpservice, inst);
}
