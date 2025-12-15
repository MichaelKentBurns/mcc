Original Makefile from google search: create a Makefile for C program with src/ and /includes dirs

# --- Project Variables ---

# The name of the final executable
TARGET := myprogram

# Source directory, object directory, and includes directory
SRC_DIR := src
OBJ_DIR := obj
INC_DIR := includes

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -I$(INC_DIR)
# Linker flags (e.g., -lm for math library if needed)
LDFLAGS :=

# Find all C source files in the source directory
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Generate corresponding object file names (e.g., src/main.c -> obj/main.o)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# --- Targets ---

# Default target: builds the executable
all: $(TARGET)

# Rule to create the executable from object files
$(TARGET): $(OBJS) | $(OBJ_DIR)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete."

# Rule to create the object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Rule to compile .c files into .o files (Pattern Rule)
# Dependencies: The source file and any header files it includes.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# --- Utility Targets ---

# Clean up build artifacts
clean:
	@echo "Cleaning up build artifacts..."
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Clean complete."

.PHONY: all clean