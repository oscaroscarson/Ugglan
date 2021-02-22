# Makefile for building python package of tools
# and running unit tests - in docker container.
#

TOOLS_ROOT = /$(shell pwd)/
TOOLS_ROOT_MNT = /tools

.PHONY: build_pkg_and_test

build_pkg_and_test:
	docker build . -t tools
	docker run tools python -m pytest
