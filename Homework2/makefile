CC = gcc
FLAGS=lpthread

object1 = output1
object2 = output2

op1: 
	$(CC) -o $(object1) process1.c -$(FLAGS)

op2: 
	$(CC) -o $(object2) process2.c -$(FLAGS)
clean:
	rm $(object1) $(object2)