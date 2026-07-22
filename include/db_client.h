#ifndef __DB_CLIENT_H
#define __DB_CLIENT_H

#include "app_context.h"
#include <stddef.h>
#include <stdint.h>

int32_t db_client_ping(app_context_t *ctx);
int32_t db_client_get_version(app_context_t *ctx, char *out_buffer,
                              size_t buffer_size);

#endif