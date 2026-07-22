#include "../include/api_server.h"
#include "../include/db_client.h"
#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct api_server {
  struct MHD_Daemon *daemon;
};

static int32_t send_json_response(struct MHD_Connection *connection,
                                  uint32_t status_code,
                                  const char *json_payload) {
  struct MHD_Response *response;
  int32_t ret;

  response = MHD_create_response_from_buffer(
      strlen(json_payload), (void *)json_payload, MHD_RESPMEM_MUST_COPY);
  if (!response)
    return MHD_NO;

  MHD_add_response_header(response, "Content-Type", "application/json");
  ret = MHD_queue_response(connection, status_code, response);
  MHD_destroy_response(response);

  return ret;
}

static enum MHD_Result
request_handler(void *cls, struct MHD_Connection *connection, const char *url,
                const char *method, const char *version,
                const char *upload_data, size_t *upload_data_size,
                void **con_cls) {
  (void)version;
  (void)upload_data;
  (void)upload_data_size; // Unused in this GET-only example

  static int dummy;

  if (&dummy != *con_cls) {
    *con_cls = &dummy;
    return MHD_YES;
  }
  *con_cls = NULL;

  app_context_t *ctx = (app_context_t *)cls;

  // GET /api/v1/health
  if (strcmp(method, "GET") == 0 && strcmp(url, "/api/v1/health") == 0) {
    if (db_client_ping(ctx) == 0) {
      return send_json_response(
          connection, MHD_HTTP_OK,
          "{\"status\": \"healthy\", \"database\": \"connected\"}");
    } else {
      return send_json_response(
          connection, MHD_HTTP_SERVICE_UNAVAILABLE,
          "{\"status\": \"unhealthy\", \"database\": \"disconnected\"}");
    }
  }

  // GET /api/v1/db-version
  if (strcmp(method, "GET") == 0 && strcmp(url, "/api/v1/db-version") == 0) {
    char db_ver[512] = {0};

    if (db_client_get_version(ctx, db_ver, sizeof(db_ver)) == 0) {
      return send_json_response(connection, MHD_HTTP_OK, db_ver);
    } else {
      return send_json_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR,
                                "{\"error\": \"Database query failed\"}");
    }
  }

  // 404 Fallback
  return send_json_response(connection, MHD_HTTP_NOT_FOUND,
                            "{\"error\": \"Not found\"}");
}

api_server_t *api_server_start(uint16_t port, app_context_t *ctx) {
  api_server_t *server = calloc(1, sizeof(api_server_t));
  if (!server)
    return NULL;

  server->daemon = MHD_start_daemon(MHD_USE_EPOLL_INTERNALLY, port, NULL, NULL,
                                    &request_handler, ctx, MHD_OPTION_END);

  if (!server->daemon) {
    free(server);

    return NULL;
  }

  return server;
}

void api_server_stop(api_server_t *server) {
  if (server) {
    if (server->daemon) {
      MHD_stop_daemon(server->daemon);
    }

    free(server);
  }
}