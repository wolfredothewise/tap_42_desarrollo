CC          = gcc
CFLAGS      = -Wall -Wextra -Werror -g -Iserver/include -Iserver/lib/cjson
CJSON_FLAGS = -Wall -Wextra -g -Iserver/lib/cjson

SERVER_BIN  = server_bin
CLI_BIN     = cli_bin

SERVER_SRCS = $(wildcard server/src/*.c)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)

CJSON_SRC   = server/lib/cjson/cJSON.c
CJSON_OBJ   = $(CJSON_SRC:.c=.o)

CLI_SRCS    = $(wildcard cli_client/*.c)
CLI_OBJS    = $(CLI_SRCS:.c=.o)

WORLD_FILE  = data/dummy_world.json

all: $(SERVER_BIN) $(CLI_BIN)

$(SERVER_BIN): $(SERVER_OBJS) $(CJSON_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_OBJS) $(CJSON_OBJ)

$(CLI_BIN): $(CLI_OBJS)
ifneq ($(CLI_SRCS),)
	$(CC) $(CFLAGS) -o $(CLI_BIN) $(CLI_OBJS)
else
	@echo "Aviso: cli_client/ vacio todavia, saltando compilacion de $(CLI_BIN)."
endif

server/src/%.o: server/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(CJSON_OBJ): $(CJSON_SRC)
	$(CC) $(CJSON_FLAGS) -c $< -o $@

cli_client/%.o: cli_client/%.c
	$(CC) $(CFLAGS) -c $< -o $@

install:
	@which gcc > /dev/null || (echo "Error: falta gcc" && exit 1)
	@which valgrind > /dev/null || (echo "Aviso: instala valgrind para auditar memoria")
	@echo "Dependencias verificadas."

run-server: $(SERVER_BIN)
	./$(SERVER_BIN) $(WORLD_FILE)

run-client: $(CLI_BIN)
	./$(CLI_BIN)

run-client-gui:
	@echo "Pendiente: comando GUI de tu companero"

lint:
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=warning,style,performance,portability --suppress=missingIncludeSystem server/src/; \
	else \
		echo "cppcheck no instalado. Instala cppcheck para el linting obligatorio."; \
	fi

clean:
	rm -f $(SERVER_OBJS) $(CJSON_OBJ) $(CLI_OBJS) $(SERVER_BIN) $(CLI_BIN)

re: clean all

.PHONY: all install run-server run-client run-client-gui lint clean re
