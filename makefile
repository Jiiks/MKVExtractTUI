CC = gcc
CFLAGS = -Wall -lncurses

# Test path to use for testing. Should have at least one mkv file with subtitles in it.
# at the top so it's easy to change.
TEST_PATH = ""

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))

BIN = mkvextracttui

# make <target> debug=1
ifdef debug
CFLAGS += -g -DDEBUG
endif

# build and run
bar: main
	$(BINDIR)/$(BIN) -i $(TEST_PATH)

build: main

main: $(OBJ)
	$(CC) -o $(BINDIR)/$(BIN) $(OBJ) $(CFLAGS)
	cp $(SRCDIR)/config.cfg $(BINDIR)/config.cfg

create_dirs:
	mkdir -p $(OBJDIR) $(BINDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | create_dirs
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/* $(BINDIR)/*

reset:
	cp $(SRCDIR)/config.default.cfg $(SRCDIR)/config.cfg

# Tests

TESTDIR = tests
TESTSRC = $(filter-out $(SRCDIR)/main.c, $(wildcard $(SRCDIR)/*.c))
TESTOBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(TESTSRC))

TEST_FS = testfs
TEST_JSON = testjson

testfs: $(TESTOBJ) $(OBJDIR)/$(TEST_FS).o
	$(CC) -o $(BINDIR)/$(TEST_FS) $(TESTOBJ) $(OBJDIR)/$(TEST_FS).o $(CFLAGS)
	$(BINDIR)/$(TEST_FS) $(TEST_PATH) ".mkv"

$(OBJDIR)/$(TEST_FS).o: $(TESTDIR)/$(TEST_FS).c
	$(CC) -c $(TESTDIR)/$(TEST_FS).c -o $(OBJDIR)/$(TEST_FS).o 

testui: $(TESTOBJ) $(OBJDIR)/testui.o
	$(CC) -o $(BINDIR)/testui $(TESTOBJ) $(OBJDIR)/testui.o $(CFLAGS)
	$(BINDIR)/testui

testjson: $(TESTOBJ) $(OBJDIR)/$(TEST_JSON).o
	$(CC) -o $(BINDIR)/$(TEST_JSON) $(TESTOBJ) $(OBJDIR)/$(TEST_JSON).o $(CFLAGS)
	$(BINDIR)/$(TEST_JSON) $(TEST_PATH) ".mkv"

$(OBJDIR)/$(TEST_JSON).o: $(TESTDIR)/$(TEST_JSON).c
	$(CC) -c $(TESTDIR)/$(TEST_JSON).c -o $(OBJDIR)/$(TEST_JSON).o 
