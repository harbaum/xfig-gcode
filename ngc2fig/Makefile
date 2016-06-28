# quite minimalistic makefile

APP = ngc2fig

all: $(APP)

$(APP): $(APP).c
	gcc -Wall -o $@ $< -lm

install:
	install $(APP) $(DESTDIR)/usr/bin

clean:
	rm -f ngc2fig *~ *.o *.fig *.ngc
