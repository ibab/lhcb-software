projs = A B C D
PWD := $(shell pwd)

CMAKE_PREFIX_PATH:=$(PWD):$(PWD)/../../..

all: $(patsubst %,build-%,$(projs))

B/build: build-A
C/build: build-A
D/build: build-B build-C

%/build:
	mkdir -p $@
	#cd $@ ; cmake -DCMAKE_TOOLCHAIN_FILE=$(PWD)/toolchain.cmake ..
	cd $@ ; cmake ..

build-%: %/build
	cd $< && CMAKE_PREFIX_PATH=$(CMAKE_PREFIX_PATH) make install

clean: $(patsubst %,clean-%,$(projs))

clean-%:
	$(RM) -r $*/build $*/InstallArea
