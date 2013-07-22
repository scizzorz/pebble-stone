#include "util.h"
#define INT_DIGITS 5		/* enough for 64 bit integer */

char *itoa(int i) {
	/* Room for INT_DIGITS digits, - and '\0' */
	static char buf[INT_DIGITS + 2];
	char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
	if (i >= 0) {
		do {
			*--p = '0' + (i % 10);
			i /= 10;
		} while (i != 0);
		return p;
	}
	else {			/* i < 0 */
		do {
			*--p = '0' - (i % 10);
			i /= 10;
		} while (i != 0);
		*--p = '-';
	}
	return p;
}
void trim(char* ptr) {
	int i = 0, j = 0;

	while(ptr[j]!='\0') {
		if(ptr[j] == ' '){
			j++;
			ptr[i] = ptr[j];
		} else {
			i++;
			j++;
			ptr[i] = ptr[j];
		}
	}
}
