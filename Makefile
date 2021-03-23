# compiles each of the .c files individually (hacky makefile)
SRCS = $(wildcard *.c)
EXECUTABLES = $(basename $(SRCS))
OUTPUT_DIR = bin

all: $(OUTPUT_DIR) $(EXECUTABLES)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

$(EXECUTABLES): $(SRCS)
	gcc $(@:=.c) -o $(OUTPUT_DIR)/$@

clean:
	rm $(addprefix $(OUTPUT_DIR)/, $(EXECUTABLES))

