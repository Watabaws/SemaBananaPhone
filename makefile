makeit: story_tim.o control.o
	gcc -o control control.o
	gcc -o story story_tim.o

story_tim.o: story_tim.c
	gcc -c story_tim.c

control.o: control.c
	gcc -c control.c
