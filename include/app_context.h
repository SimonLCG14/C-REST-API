#ifndef __APP_CONTEXT_H
#define __APP_CONTEXT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct app_context app_context_t;

app_context_t *app_context_create(const char *db_conn_info);
void app_context_destroy(app_context_t *ctx);

#endif