#!/usr/bin/env bash

gcc -Wall -Wextra -Werror -Wpedantic -O2 \
    -o rest_api main.c src/db_client.c src/api_server.c \
    -lmicrohttpd -lpq

./rest_api