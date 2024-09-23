# ============================================================================ #
#                                    TARGETS                                   #
# ============================================================================ #

# ------------------------------- FINAL BINARY ------------------------------- #
NAME		:= ft_ping

# ------------------------------ DIRECTORY NAMES ----------------------------- #
SRC_DIR		:=	src
OBJ_DIR		:=	obj

LIB_DIR		:=	lib
INPUT_DIR	:=	input
DEBUG_DIR	:=	debug
UTILS_DIR	:=	utils

# ------------------------------ SUBDIRECTORIES ------------------------------ #
SUBDIRS		:=	$(LIB_DIR) \
				$(INPUT_DIR) \
				$(UTILS_DIR) \
				$(DEBUG_DIR)

OBJ_SUBDIRS	:=	$(addprefix $(OBJ_DIR)/,$(SUBDIRS))
INC_SUBDIRS	:=	$(addprefix $(SRC_DIR)/,$(SUBDIRS))

# ---------------------------------- SOURCES --------------------------------- #
SRC_FILES	:=	main.c \
				$(UTILS_DIR)/ft_string.c

SRC			:=	$(addprefix $(SRC_DIR)/,$(SRC_FILES))
OBJ			:=	$(addprefix $(OBJ_DIR)/,$(SRC_FILES:.c=.o))
DEP			:=	$(addprefix $(OBJ_DIR)/,$(SRC_FILES:.c=.d))

# --------------------------------- COMPILER --------------------------------- #
CXX			:=	gcc

MACROS		:=
DEFINES		:=	$(addprefix -D,$(MACROS))

INCLUDES	:=	$(addprefix -I./,$(INC_SUBDIRS))

CFLAGS		:= 	-Wall \
				-Wextra \
				-Werror \
				-MMD \
				-MP \
				-O3 \
				$(INCLUDES) \
				$(DEFINES)

LDFLAGS		:=	-lpthread \
				# -lpcap

# ----------------------------------- MISC ----------------------------------- #
RM			:=	rm -rf

# ============================================================================ #
#                                     RULES                                    #
# ============================================================================ #

.PHONY: all
all: $(NAME)

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