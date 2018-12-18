CC = gcc
DEBUG = -g
CFLAGS = -Wall -pedantic -std=c99 -pthread -c $(DEBUG)
LFLAGS = -Wall -pedantic -std=c99 -pthread $(DEBUG)

PA04: Process_Parser.o Config_Parser.o Driver.o Utility.o Output_Handler.o Scheduler.o Timer.o CPU.o MMU.o IOThread.o
	$(CC) $(LFLAGS) Process_Parser.o Config_Parser.o Driver.o Utility.o Output_Handler.o Scheduler.o Timer.o CPU.o MMU.o IOThread.o -o PA04

Process_Parser.o: Process_Parser.c Process_Parser.h Structures.h Utility.c Utility.h
	$(CC) $(CFLAGS) Process_Parser.c
	
Config_Parser.o: Config_Parser.c Config_Parser.h Structures.h Utility.c Utility.h
	$(CC) $(CFLAGS) Config_Parser.c
	
Driver.o: Driver.c Driver.h Structures.h Process_Parser.h Config_Parser.h
	$(CC) $(CFLAGS) Driver.c

Utility.o: Utility.c Utility.h
	$(CC) $(CFLAGS) Utility.c

Output_Handler.o: Output_Handler.c Output_Handler.h Structures.h
	$(CC) $(CFLAGS) Output_Handler.c
	
Scheduler.o: Scheduler.c Scheduler.h Structures.h
	$(CC) $(CFLAGS) Scheduler.c
	
Timer.o: Timer.c Timer.h
	$(CC) $(CFLAGS) Timer.c
	
CPU.o: CPU.c CPU.h
	$(CC) $(CFLAGS) CPU.c

MMU.o: MMU.c MMU.h
	$(CC) $(CFLAGS) MMU.c
	
IOThread.o: IOThread.c IOThread.h
	$(CC) $(CFLAGS) IOThread.c
	
clean:
	\rm *.o PA04
	
target: dependencies
	action