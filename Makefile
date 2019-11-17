# c version: gnu11 

SRC_DIR := ./src
INCLUDE_DIR := ./include
BUILD_DIR := ./build
CC := gcc

fileserver: $(BUILD_DIR)/main.o $(BUILD_DIR)/server.o 
	$(CC) -o fileserver $(BUILD_DIR)/main.o $(BUILD_DIR)/server.o
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c $(INCLUDE_DIR)/server.h
	$(CC) -c -o $(BUILD_DIR)/main.o $(SRC_DIR)/main.c
$(BUILD_DIR)/server.o: $(SRC_DIR)/server.c $(INCLUDE_DIR)/server.h
	$(CC) -c -o $(BUILD_DIR)/server.o $(SRC_DIR)/server.c

clean:
	rm -f $(BUILD_DIR)/*.o
