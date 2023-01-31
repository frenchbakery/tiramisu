SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
RUN_DIR = run
WORKSPACE_NAME = $(shell basename ${PWD})

CC = g++
CFLAGS = -g -I$(INCLUDE_DIR) -I$(SRC_DIR)
LIBS = -lpthread -lwallaby

USER = access
HOST = 10.42.0.149

SOURCES = $(shell find $(SRC_DIR) -name '*.cpp')
OBJECTS_TEMP = $(patsubst %.cpp,%.o,$(SOURCES))
OBJECTS = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(OBJECTS_TEMP))
EXECUTABLE = $(RUN_DIR)/main

# create executable file from all object files
$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(RUN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

# create object file for every source file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

remote_start: 
	ssh $(USER)@$(HOST) "bash -c \"cd projects/$(WORKSPACE_NAME) && $(RUN_DIR)/main\""

remote_build:
	ssh $(USER)@$(HOST) "bash -c \"cd projects/$(WORKSPACE_NAME) && make\""

copy_files:
	ssh $(USER)@$(HOST) "rm -rf projects/$(WORKSPACE_NAME)/* && mkdir -p projects/$(WORKSPACE_NAME)/$(SRC_DIR) && mkdir -p projects/$(WORKSPACE_NAME)/$(INCLUDE_DIR)"
	scp -r ../$(WORKSPACE_NAME)/$(SRC_DIR) $(USER)@$(HOST):projects/$(WORKSPACE_NAME)/$(SRC_DIR)
	scp -r ../$(WORKSPACE_NAME)/$(INCLUDE_DIR) $(USER)@$(HOST):projects/$(WORKSPACE_NAME)/$(INCLUDE_DIR)
	scp -r ../$(WORKSPACE_NAME)/Makefile $(USER)@$(HOST):projects/$(WORKSPACE_NAME)/Makefile

shell:
	ssh $(USER)@$(HOST)

mount:
	sshfs -o allow_other access@raspberrypi.local:/ /media/melektron/kipr_mounts/root_mount

unmount:
	sudo umount /media/melektron/kipr_mounts/root_mount

# remove all subfolders and files of the object dir and the executable
clean:
	rm -rf $(EXECUTABLE) $(OBJ_DIR)/*