SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
RUN_DIR = run

CC = g++
CFLAGS = -g -I$(SRC_DIR)
LIBS = -g

USER = access
HOST = 10.42.0.149

SOURCES = $(shell find $(SRC_DIR) -name '*.cpp')
OBJECTS_TEMP = $(patsubst %.cpp,%.o,$(SOURCES))
OBJECTS = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(OBJECTS_TEMP))
EXECUTABLE = $(RUN_DIR)/main

all: deploy

start: $(EXECUTABLE)
	${EXECUTABLE}

# create executable file from all object files
$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(RUN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

# create object file for every source file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

deploy:
	scp ./../$(shell basename $PWD) access@$(HOST):projects/$(shell basename $PWD)


mount:
	sshfs -o allow_other access@raspberrypi.local:/ /media/melektron/kipr_mounts/root_mount

unmount:
	sudo umount /media/melektron/kipr_mounts/root_mount

# remove all subfolders and files of the object dir and the executable
clean:
	rm -rf $(EXECUTABLE) $(OBJ_DIR)/*