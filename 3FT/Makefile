#--------------------------------------------------------------------
# Makefile for Assignment 4
# Invoke with the command:
# 	make -f Makefile.sampleft
# Authors: Mirabelle Weinbach and John Wallace
#--------------------------------------------------------------------

CC=gcc217 -g

all: ft

clean:
	rm -f ft
	rm -f node

clobber: clean
	rm -f ft_client.o *~
	rm -f node_client.o *~
	rm -f *.o *~

node: nodeFT.o node_client.o dynarray.o path.o
	$(CC) nodeFT.o node_client.o dynarray.o path.o -o node

ft: ft.o ft_client.o nodeFT.o dynarray.o path.o
	$(CC) ft.o ft_client.o nodeFT.o dynarray.o path.o -o ft

ft_client.o: ft_client.c ft.h dynarray.h a4def.h
	$(CC) -c ft_client.c

node_client.o: node_client.c nodeFT.h path.h
	$(CC) -c node_client.c
	
nodeFT.o: nodeFT.c dynarray.h nodeFT.h
	$(CC) -c nodeFT.c

path.o: path.c path.h dynarray.h
	$(CC) -c path.c
	
dynarray.o: dynarray.c 
	$(CC) -c dynarray.c


