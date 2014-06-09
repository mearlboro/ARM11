#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#define INT_DIGITS (19) /* 64 bit int */
char *itoa(int i)
{
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

int main(int argc, char **argv)
{
   printf("%s\n", itoa(0));
   char *cpy;
   cpy = itoa(32);
   printf("%s\n", cpy);
   return 0;
}
