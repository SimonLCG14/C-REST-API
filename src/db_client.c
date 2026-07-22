#include "../include/db_client.h"
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct app_context {
  PGconn *db_conn;
};

app_context_t *app_context_create(const char *db_conn_info) {
  if (!db_conn_info)
    return NULL;

  app_context_t *ctx = calloc(1, sizeof(app_context_t));

  if (!ctx) {
    return NULL;
  }

  ctx->db_conn = PQconnectdb(db_conn_info);

  if (PQstatus(ctx->db_conn) != CONNECTION_OK) {
    fprintf(stderr, "[DB_CLIENT ERROR] PQconnectdb failed: %s\n",
            PQerrorMessage(ctx->db_conn));
    PQfinish(ctx->db_conn);
    free(ctx);

    return NULL;
  }
  return ctx;
}

void app_context_destroy(app_context_t *ctx) {
  if (ctx) {
    if (ctx->db_conn) {
      PQfinish(ctx->db_conn);
    }
    free(ctx);
  }
}

int32_t db_client_ping(app_context_t *ctx) {
  if (!ctx || !ctx->db_conn) {
    return -1;
  }

  return (PQstatus(ctx->db_conn) == CONNECTION_OK) ? 0 : -1;
}

int32_t db_client_get_version(app_context_t *ctx, char *out_buffer,
                              size_t buffer_size) {
  if (!ctx || !out_buffer || buffer_size == 0)
    return -1;

  out_buffer[0] = '\0';

  PGresult *res = PQexecParams(ctx->db_conn, "SELECT version();", 0, NULL, NULL,
                               NULL, NULL, 0);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    PQclear(res);
    return -1;
  }

  const char *version_str = PQgetvalue(res, 0, 0);

  snprintf(out_buffer, buffer_size, "{\"version\": \"%s\"}", version_str);

  PQclear(res);

  return 0;
}
