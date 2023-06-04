#
# makefile for mysh
#
CC=gcc
CFLAGS=-g

mysh mysh.o splitline.o execute.o
	gcc -c mysh.c
	gcc -c splitline.c
	gcc -c execute.c
	gcc -g -o mysh mysh.o splitline.o execute.o