GREP_OBJS = $(ROOT_DIR)/bin/grep/grep.o

GREP = $(TARGET_DIR)/$(ROOT_DIR)/bin/grep

PROGS += $(GREP)
CLEAN += $(GREP_OBJS)

$(GREP): $(GREP_OBJS)
	$(LD) -Ttext=100 $(GREP_OBJS) -o $(GREP) $(LDFLAGS) -lewokc -lc
