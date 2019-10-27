# c version: gnu11 

SRC_DIR := ./src
BUILD_DIR := ./build
CC := gcc

fileserver: $(BUILD_DIR)/main.o
	$(CC) -o fileserver $(BUILD_DIR)/main.o
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) -c -o $(BUILD_DIR)/main.o $(SRC_DIR)/main.c

clean:
	rm -f $(BUILD_DIR)/*.o
