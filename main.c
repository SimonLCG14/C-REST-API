#include "include/api_server.h"
#include "include/app_context.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t keep_running = 1;

void sigint_handler(int signum) {
  (void)signum;
  keep_running = 0;
}

int main(void) {
  struct sigaction act;

  act.sa_handler = sigint_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGTERM, &act, NULL);

  const char *db_conn_str = "host=localhost port=5432 dbname=rest-c-db "
                            "user=postgres password=postgres";
  uint16_t port = 8080;

  printf("Initialize Application Context...\n");

  app_context_t *ctx = app_context_create(db_conn_str);

  if (!ctx) {
    fprintf(stderr, "Fatal: Could not initialize database context.\n");
    return EXIT_FAILURE;
  }

  printf("Start API Server on port %u...\n", port);
  api_server_t *server = api_server_start(port, ctx);

  if (!server) {
    fprintf(stderr, "Fatal: Could not start HTTP server.\n");
    app_context_destroy(ctx);
    return EXIT_FAILURE;
  }

  printf("Server running. Press Ctrl+C to stop.\n");

  // Main Event Loop
  while (keep_running) {
    sleep(1);
  }

  printf("\nShutting down...\n");

  api_server_stop(server);
  app_context_destroy(ctx);

  printf("Shutdown complete.\n");
  return EXIT_SUCCESS;
}