# ============================================================================ #
#                                    TARGETS                                   #
# ============================================================================ #

# ------------------------------- FINAL BINARY ------------------------------- #
NAME		:= ft_ping

# ------------------------------ DIRECTORY NAMES ----------------------------- #
SRC_DIR		:=	src
OBJ_DIR		:=	obj

UTILS_DIR	:=	utils

# ------------------------------ SUBDIRECTORIES ------------------------------ #
SUBDIRS		:=	$(UTILS_DIR)

OBJ_SUBDIRS	:=	$(addprefix $(OBJ_DIR)/,$(SUBDIRS))
INC_SUBDIRS	:=	$(addprefix $(SRC_DIR)/,$(SUBDIRS))

# ---------------------------------- SOURCES --------------------------------- #
SRC_FILES	:=	main.c \
				$(UTILS_DIR)/log.c \
				$(UTILS_DIR)/wrapper.c

SRC			:=	$(addprefix $(SRC_DIR)/,$(SRC_FILES))
OBJ			:=	$(addprefix $(OBJ_DIR)/,$(SRC_FILES:.c=.o))
DEP			:=	$(addprefix $(OBJ_DIR)/,$(SRC_FILES:.c=.d))

# -------------------------------- COMPILATION ------------------------------- #
## Compiler
CXX			:=	gcc

## Flags
MACROS		:=
DEFINES		:=	$(addprefix -D,$(MACROS))

INCLUDES	:=	$(addprefix -I./,$(INC_SUBDIRS))

CFLAGS		:=	-MMD \
				-MP

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
endif

CFLAGS		+=	$(INCLUDES) \
				$(DEFINES)

LDFLAGS		:=

# ----------------------------------- MISC ----------------------------------- #
RM			:=	rm -rf

# ============================================================================ #
#                                     RULES                                    #
# ============================================================================ #

.PHONY: all
all: $(NAME)

test:
	echo $(CFLAGS)

-include $(DEP)

# Compile binary
$(NAME):   $(OBJ)
	@$(CXX) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)
	@echo "\`$(NAME)\` successfully created."

# Compile obj files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR) $(OBJ_SUBDIRS)
	@echo "Compiling file $<..."
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