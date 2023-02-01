
# include environment configuration that includes deployment
# host and user name
include env.mk


# project structure
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
RUN_DIR = run
MOUNT_FOLDER = mount
WORKSPACE_NAME = $(shell basename ${PWD})

# C++ configuration
CC = g++
CFLAGS = -g -I$(INCLUDE_DIR) -I$(SRC_DIR) -std=c++11
LIBS = -lpthread -lwallaby

# files and compliation results
SOURCES = $(shell find $(SRC_DIR) -name '*.cpp')
OBJECTS_TEMP = $(patsubst %.cpp,%.o,$(SOURCES))
OBJECTS = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(OBJECTS_TEMP))
EXECUTABLE = $(RUN_DIR)/main


## == Compilation

# create executable file from all object files
$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(RUN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

# create object file for every source file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


## == Remote target management

# run main binary remote target
remote_start: 
	ssh $(USER)@$(HOST) "bash -c \"cd projects/$(WORKSPACE_NAME) && sudo $(RUN_DIR)/main\""

# envoke build on remote target
remote_build:
	ssh $(USER)@$(HOST) "bash -c \"cd projects/$(WORKSPACE_NAME) && make\""

# copy sources and Makefile to remote target
copy_files:
	ssh $(USER)@$(HOST) "rm -rf projects/$(WORKSPACE_NAME)/* && mkdir -p projects/$(WORKSPACE_NAME)/$(SRC_DIR) && mkdir -p projects/$(WORKSPACE_NAME)/$(INCLUDE_DIR)"
	scp -r ../$(WORKSPACE_NAME)/$(SRC_DIR) $(USER)@$(HOST):projects/$(WORKSPACE_NAME)/
	scp -r ../$(WORKSPACE_NAME)/$(INCLUDE_DIR) $(USER)@$(HOST):projects/$(WORKSPACE_NAME)/$(INCLUDE_DIR)
	scp -r ../$(WORKSPACE_NAME)/Makefile $(USER)@$(HOST):projects/$(WORKSPACE_NAME)/Makefile
	scp -r ../$(WORKSPACE_NAME)/env.mk $(USER)@$(HOST):projects/$(WORKSPACE_NAME)/env.mk

# open shell on remote target
shell:
	ssh $(USER)@$(HOST)


## == shortcuts and convenience features

# quickly copy files to target, compile and start on target
start:
	make copy_files
	make remote_build
	make remote_start

# quickly copy files to target and compile them
build:
	make copy_files
	make remote_build

# mount the 
mount:
	mkdir -p $(MOUNT_FOLDER)
	sshfs -o allow_other $(USER)@$(HOST):/ $(MOUNT_FOLDER)

unmount:
	sudo umount $(MOUNT_FOLDER)

# remove all subfolders and files of the object dir and the executable
clean:
	rm -rf $(EXECUTABLE) $(OBJ_DIR)/*