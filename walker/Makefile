CFLAGS += -g
appname = walker 
obj-files = $(addsuffix .o,$(basename $(wildcard *.c)))

$(appname): $(obj-files)
	gcc $(LDFLAGS) $(obj-files) -o $@

.PHONY: clean
clean:
	rm -f *.o *~ $(appname)

