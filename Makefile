#regtool-2.0 Makefile sukk@cloud-times.com

CC=gcc
CFLAGS=-Wall -O3 `pkg-config --libs --cflags gtk+-2.0`
SERVER=regtool_server
CLIENT=regtool_client

all:$(SERVER) $(CLIENT)

$(SERVER):$(SERVER).o
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT):$(CLIENT).o
	$(CC) $(CFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@

app:all
	rm -f regtool.app
	strip $(CLIENT)
	strip $(SERVER)
	ln -s $(CLIENT) regset
	ln -s $(CLIENT) regget
	ln -s $(CLIENT) regdel
	ln -s $(CLIENT) regrm
	mv $(SERVER) $(CLIENT) regget regset regdel regrm app_path/bin
	mksquashfs app_path regtool.app

clean:
	rm -f *.o $(SERVER) $(CLIENT) regget regset regdel regrm regtool.app
