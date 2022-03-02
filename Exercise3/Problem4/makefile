CC = gcc
FLAGS=lpthread

object1 = output1
object2 = output2

op1: 
	$(CC) -o $(object1) heap_mq.c -$(FLAGS)

op2: 
	$(CC) -o $(object2) posix_mq.c -$(FLAGS)
clean:
	rm $(object1) $(object2)
