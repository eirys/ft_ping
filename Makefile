# ============================================================================ #
#                                    TARGETS                                   #
# ============================================================================ #

# ------------------------------- FINAL BINARY ------------------------------- #
NAME		:= ft_ping

# ------------------------------ DIRECTORY NAMES ----------------------------- #
SRC_DIR		:=	src
OBJ_DIR		:=	obj

UTILS_DIR	:=	utils
INPUT_DIR	:=	input
NETWORK_DIR	:=	network

# ------------------------------ SUBDIRECTORIES ------------------------------ #
SUBDIRS		:=	$(UTILS_DIR) \
				$(INPUT_DIR) \
				$(NETWORK_DIR)

OBJ_SUBDIRS	:=	$(addprefix $(OBJ_DIR)/,$(SUBDIRS))
INC_SUBDIRS	:=	$(addprefix $(SRC_DIR)/,$(SUBDIRS))

# ---------------------------------- SOURCES --------------------------------- #
SRC_FILES	:=	main.c \
				$(UTILS_DIR)/log.c \
				$(UTILS_DIR)/wrapper.c \
				$(UTILS_DIR)/set_alarm.c \
				$(INPUT_DIR)/options.c \
				$(INPUT_DIR)/callbacks.c \
				$(NETWORK_DIR)/raw_socket.c \
				$(NETWORK_DIR)/receive_msg.c \
				$(NETWORK_DIR)/send_request.c \
				$(NETWORK_DIR)/stats.c

SRC			:=	$(addprefix $(SRC_DIR)/,$(SRC_FILES))
OBJ			:=	$(addprefix $(OBJ_DIR)/,$(SRC_FILES:.c=.o))
DEP			:=	$(addprefix $(OBJ_DIR)/,$(SRC_FILES:.c=.d))

# -------------------------------- COMPILATION ------------------------------- #
## Compiler
CXX			:=	gcc

## Flags
MACROS		:=

INCLUDES	:=	$(addprefix -I./,$(INC_SUBDIRS))

CFLAGS		:=	-MMD \
				-MP \
				-std=gnu17

# Enable mode
debug := 1

ifdef error
CFLAGS		+=	-Wall \
				-Wextra \
				-Werror
endif

ifdef optimize
CFLAGS		+=	-O3
endif

ifdef debug
CFLAGS		+=	-g
MACROS		+=	__DEBUG
IS_DEBUG	:= " (debug mode)"
endif

DEFINES		:=	$(addprefix -D,$(MACROS))

CFLAGS		+=	$(INCLUDES) \
				$(DEFINES)

LDFLAGS		:=	-lm

# ----------------------------------- MISC ----------------------------------- #
RM			:=	rm -rf

COMPOSE		:=	docker compose

# ============================================================================ #
#                                     RULES                                    #
# ============================================================================ #

.PHONY: all
all: $(NAME) copy

.PHONY: copy
copy: ping_output
	cp $(NAME) ping_output

-include $(DEP)

# Compile binary
$(NAME):   $(OBJ)
	@$(CXX) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)
	@echo "\`$(NAME)\` successfully created."

# Compile obj files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR) $(OBJ_SUBDIRS)
	@echo "Compiling file $<"$(IS_DEBUG)"..."
	@$(CXX) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@$(RM) $(OBJ_DIR)
	@echo "Cleaning object files and dependencies."

.PHONY: fclean
fclean: clean
	@$(RM) $(NAME)
	@echo "Removed $(NAME)."

.PHONY: re
re: fclean all

# ---------------------------------- DOCKER ---------------------------------- #

.PHONY: up
up: ping_output graphics_permission
	$(COMPOSE) up -d

ping_output:
	@mkdir -p ping_output

.PHONY: graphics_permission
graphics_permission:
	@xhost +local:docker

.PHONY: down
down:
	$(COMPOSE) down

.PHONY: clear
clear: down
	docker volume rm -f $(shell docker volume ls -q)
	docker image rm -f $(shell docker image ls -aq)

.PHONY: dre
dre: clear up

.PHONY: exec
exec: up
	docker exec -ti debian42 bash