# c version: gnu11 

SRC_DIR := ./src
BUILD_DIR := ./build
CC := gcc

objects := $(BUILD_DIR)/main.o $(BUILD_DIR)/tools.o $(BUILD_DIR)/http.o $(BUILD_DIR)/url_parser.o

fileserver:$(objects)
	$(CC) -o fileserver $(objects)
	
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/http.h
	$(CC) -c $(SRC_DIR)/main.c  -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/tools.o: $(SRC_DIR)/tools.c $(SRC_DIR)/tools.h
	$(CC) -c  $(SRC_DIR)/tools.c -o $(BUILD_DIR)/tools.o

$(BUILD_DIR)/http.o: $(SRC_DIR)/http.c $(SRC_DIR)/http.h $(SRC_DIR)/tools.h $(SRC_DIR)/url_parser.h
	$(CC) -c $(SRC_DIR)/http.c -o $(BUILD_DIR)/http.o

$(BUILD_DIR)/url_parser.o: $(SRC_DIR)/url_parser.c $(SRC_DIR)/url_parser.h 
	$(CC) -c $(SRC_DIR)/url_parser.c -o $(BUILD_DIR)/url_parser.o

clean:
	rm -f $(BUILD_DIR)/*.o
