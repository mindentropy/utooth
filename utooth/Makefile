CC=msp430-gcc
MCU=msp430f5438a

CFLAGS=-mmcu=$(MCU) -g -Wall 
INCLUDE = -I./include
LIB_INCLUDE=/usr/msp430/include
LDFLAGS=-mmcu=$(MCU)

BIN_DIR=bin
SRC_DIR=src
INCLUDE_DIR=include
OBJ_DIR=obj


SRC_FILES := $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.c))
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

vpath %.h $(INCLUDE_DIR)
vpath %.c $(SRC_DIR)
vpath %.o $(OBJ_DIR)

all: bluetooth_init_cc2560A_2.1.o hex 


#Create hex file from .out
hex: btdev
	msp430-objcopy -O ihex $(BIN_DIR)/$< $(BIN_DIR)/$<.hex

cscope_rule:
	cscope -R -b -v -q -I$(LIB_INCLUDE)


bluetooth_init_cc2560A_2.1.o: $(SRC_DIR)/bluetooth_init_cc2560A_2.1.c
	$(CC) -mmcu=msp430f5438a $(INCLUDE) -c $^ -o $(OBJ_DIR)/$@

btdev: $(OBJ_FILES)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(BIN_DIR)/$@ $(LDFLAGS) -Wl,-Map=mem.map

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $^ -o $@

cscope_clean:
	rm -rfv *.out

clean:
	rm -rfv $(OBJ_DIR)/*.o $(BIN_DIR)/* *.map
