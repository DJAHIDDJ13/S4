#File:     Makefile for Khepera Simulator (example 1)
#Author:   Olivier MICHEL <om@alto.unice.fr>
#Date:     Tue Feb 13 11:29:50 MET 1996

#   Copyright (c) 1995, 1996
#   Olivier MICHEL
#   MAGE team, i3S laboratory,
#   CNRS, University of Nice - Sophia Antipolis

#   Permission is hereby granted to copy all or any part of
#   this program for free distribution.   The author's name
#   and this copyright notice must be included in any copy.
#   Commercial use is forbidden.


#LIBS   = -g -L/usr/lib -lX11 -lm

#CC = gcc $(LIBS)

#sim:
#	gcc -L/usr/lib -lX11 -lm SRC/robot.c SRC/world.c SRC/graphics.c SRC/khep_serial.c USER/user.c SRC/sim.c USER/fuzzy_infer.c USER/fuzzy_utils.c -o sim -lX11 -lm


#clean:
#	rm -f sim OBJ/*.o

VPATH=OBJ/:SRC/:USER/

CFLAGS = -I/usr/include \
         -O3

LIBS   = -g -L/usr/lib -lX11 -lm

cc = gcc $(CFLAGS) -c

CC = gcc $(LIBS)

sim:	sim.o robot.o world.o graphics.o khep_serial.o user.o fuzzy_infer.o
	gcc -L/usr/lib OBJ/sim.o OBJ/robot.o OBJ/world.o OBJ/graphics.o OBJ/khep_serial.o OBJ/user.o OBJ/fuzzy_infer.o OBJ/fuzzy_utils.o -o sim -lX11 -lm

sim.o: sim.c sim.h
	$(cc) SRC/sim.c -o OBJ/sim.o

robot.o: robot.c robot.h
	$(cc) SRC/robot.c -o OBJ/robot.o

world.o: world.c world.h
	$(cc) SRC/world.c -o OBJ/world.o

graphics.o: graphics.c graphics.h
	$(cc) SRC/graphics.c -o OBJ/graphics.o

khep_serial.o: khep_serial.c khep_serial.h gen_types.h
	$(cc) SRC/khep_serial.c -o OBJ/khep_serial.o

user.o: user.c user.h user_info.h include.h 
	$(cc) USER/user.c -o OBJ/user.o

fuzzy_infer.o: fuzzy_infer.c fuzzy_utils.c fuzzy_infer.h 
	$(cc) USER/fuzzy_utils.c -o OBJ/fuzzy_utils.o
	$(cc) USER/fuzzy_infer.c -o OBJ/fuzzy_infer.o

header.h: types.h graphics.h sim.h robot.h world.h
	touch SRC/header.h

clean:
	rm -f sim OBJ/*.o
