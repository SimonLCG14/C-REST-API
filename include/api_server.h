#ifndef API_SERVER_H
#define API_SERVER_H

#include "app_context.h"
#include <stdint.h>

typedef struct api_server api_server_t;

api_server_t *api_server_start(uint16_t port, app_context_t *ctx);

void api_server_stop(api_server_t *server);

#endif // API_SERVER_H