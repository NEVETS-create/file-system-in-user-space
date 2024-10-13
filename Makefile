# Define compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra `pkg-config --cflags gtk+-3.0` -Iinclude

# Define linker flags
LDFLAGS = `pkg-config --libs gtk+-3.0`

# Define directories
SRC_DIR = src
OBJ_DIR = obj

# Define the target executable (directly in the project root)
TARGET = gtk_app

# Find all .c files in the source directory
SRC_FILES = $(wildcard $(SRC_DIR)/main.c) $(wildcard $(SRC_DIR)/file_system.c)

# Convert source files to object files
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Default target to build the executable
all: $(TARGET)

# Rule to link object files into the executable
$(TARGET): $(OBJ_FILES)
# Ensure obj directory exists
	@mkdir -p $(OBJ_DIR)  
	$(CC) $(OBJ_FILES) $(CFLAGS) -o $@ $(LDFLAGS)

# Rule to compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
 # Ensure obj directory exists
	@mkdir -p $(OBJ_DIR) 
	$(CC) $(CFLAGS) -c $< -o $@

# Automatically generate dependencies
-include $(OBJ_FILES:.o=.d)

# Rule to generate dependencies
$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)  # Ensure obj directory exists
	$(CC) -M $(CFLAGS) $< > $@

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Phony targets
.PHONY: all clean
