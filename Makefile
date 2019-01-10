#******************************************************************************#
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ddinaut <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/02/26 16:31:33 by ddinaut           #+#    #+#              #
#    Updated: 2019/01/10 15:58:19 by ddinaut          ###   ########.fr        #
#                                                                              #
#******************************************************************************#

# Output file #
NAME	= packer

# Details #
CC			= gcc
FLAGS		= -Wall -Wextra -Werror
ADDFLAGS	= #-fsanitize=address

# Directories
OBJ_DIR		= .obj
SRC_DIR		= srcs
ASM_DIR		= $(SRC_DIR)/asm
INC_DIR		= includes/
LIB_PATH	= libft

LIBFT		= -L $(LIB_PATH)
LIBS		= $(LIBFT) -lft $(libft)
INCLUDES	= -I $(LIB_PATH)/$(INC_DIR) -I $(INC_DIR)

# Colors #
COL_BLACK	= \033[1;30m
COL_RED		= \033[1;31m
COL_GREEN	= \033[1;32m
COL_YELLOW	= \033[1;33m
COL_BLUE	= \033[1;34m
COL_PURPLE	= \033[1;35m
COL_WHITE	= \033[1;37m
CLEAN_LINE	= \033[K
END_COL		= \033[0;m

# Sources #
SRCS =				\
	packer.c		\
	packer_core.c	\
	infection_x32.c	\
	infection_x64.c	\
	error.c

OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
SRC_FILES = $(addprefix $(SRC_DIR)/,$(SRCS))
# Rules #
.PHONY: all clean fclean re libft

all: $(NAME)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) -o $@ $(FLAGS) $(ADDFLAGS) $(INCLUDES) -c $<

$(NAME): libft binaries $(OBJ_FILES)
	$(CC) -o $(NAME) $(FLAGS) $(ADDFLAGS) $(OBJ_FILES) $(LIBS)

libft:
	make -C libft/

binaries:
	gcc $(FLAGS) srcs/infect.c -o target
#	nasm -f elf64 $(ASM_DIR)/print.s -o $(ASM_DIR)print.o
#	ld -m elf_x86_64 $(ASM_DIR)/print.o -o $(OUTPUT_DIR)/print

clean:
	/bin/rm -f $(OBJ_FILES)
#	make -C libft/ clean

fclean: clean
	/bin/rm -f $(NAME)
#	make -C libft/ fclean

re: fclean all
