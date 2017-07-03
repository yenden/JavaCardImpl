CC	:=	gcc
LD	:=	gcc

MODULES	:=	nvm jcvm/installer	jcvm	/
SRC_DIR	:=	$(addprefix	src/,$(MODULES))
BUILD_DIR	:=	$(addprefix build/,$(MODULES))

SRC	:=	$(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
OBJ	:=	$(patsubst src/%.c,build/%.o,$(SRC))
INCLUDES	:=	$(addprefix -I,$(SRC_DIR))

vpath	%.c	$(SRC_DIR)

define	make-goal
$1/%.o:	%.c
	$(CC) $(INCLUDES) -c $$< -o $$@
endef

.PHONY:	all	checkdirs	clean

all:	checkdirs	build/jcvm.exe

build/jcvm.exe:	$(OBJ)
	$(LD) $^ -o $@


checkdirs:	$(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir)))) 
