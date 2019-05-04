#include <termios.h>
#include <stdio.h>

static struct termios told, tnew;
void initTermios() {
	tcgetattr(0, &told);
	tnew = told;
	tnew.c_lflag &= ~ICANON;
	tnew.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &tnew);
}

void resetTermios() {
	tcsetattr(0, TCSANOW, &told);
}

char getch(){
	char ch;
	initTermios();

	ch = getchar();

	resetTermios();
	return ch;
}

void getch_key(){
	char c;
	printf("(getch example) please type a letter...");
	c = getch();
	printf("\nYou typed: %c\n", c);
}

/* Let's test it out */
int main(void) {
	getch_key();
	getch_key();
	getch_key();
	return 0;
} 