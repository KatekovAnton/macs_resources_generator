
ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    detected_OS := Windows
else
    detected_OS := $(shell uname)  # same as "uname -s"
	ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
endif

.PHONY: configure_project_mac
configure_project_mac:
	mkdir -p buildXC && cd buildXC && cmake -G Xcode ..
