CC = gcc
CFLAGS = -Wall -Wextra
NARWAL_TZ_DIR = ./src/narwal_thermal_zones
NARWAL_RAM_DIR = ./src/narwal_ram

CPPFLAGS_TZ =  -I$(NARWAL_TZ_DIR) 
CPPFLAGS_RAM = -I$(NARWAL_RAM_DIR) 

test_tz: $(NARWAL_TZ_DIR)/narwal_thermal_zones.c  ./tests/test_tz.c 
	mkdir -p ./build/tests/
	$(CC) $(CPPFLAGS_TZ) -o ./build/tests/basic_test $(NARWAL_TZ_DIR)/narwal_thermal_zones.c  ./tests/test_tz.c  

test_ram: $(NARWAL_RAM_DIR)/narwal_ram.c ./tests/test_ram_basic.c
	mkdir -p ./build/tests/
	$(CC) $(CPPFLAGS_RAM) -o ./build/tests/test_ram_basic $(NARWAL_RAM_DIR)/narwal_ram.c  ./tests/test_ram_basic.c


tz_inspect: ./examples/narwal_tz_inspect/narwal_tz_inspect.c $(NARWAL_TZ_DIR)/narwal_thermal_zones.c
	mkdir -p ./build/tests/examples/
	$(CC) $(CPPFLAGS_TZ) -o ./build/tests/examples/tz_inspect $(NARWAL_TZ_DIR)/narwal_thermal_zones.c ./examples/narwal_tz_inspect/narwal_tz_inspect.c

