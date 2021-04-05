# compiles each of the .c and .asm files individually (hacky! makefile)
SRCS = $(wildcard *.c)
EXECUTABLES = $(basename $(SRCS))
ASM_SRCS = $(wildcard *.asm)
ASM_EXES = $(basename $(ASM_SRCS))
OUTPUT_DIR = bin


all: $(OUTPUT_DIR) $(EXECUTABLES) $(ASM_EXES)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

$(EXECUTABLES): $(SRCS)
	gcc -Wall $(@:=.c) -o $(OUTPUT_DIR)/$@

$(ASM_EXES): $(ASM_SRCS)
	nasm -f elf64 $(@:=.asm) -o $(OUTPUT_DIR)/$(@:=.o)
	ld -o $(OUTPUT_DIR)/$@ $(OUTPUT_DIR)/$(@:=.o)

clean:
	rm $(addprefix $(OUTPUT_DIR)/, $(EXECUTABLES))
	rm $(addprefix $(OUTPUT_DIR)/, $(ASM_EXES))
	rm $(addprefix $(OUTPUT_DIR)/, $(ASM_EXES:=.o))

