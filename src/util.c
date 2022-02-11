#include "util.h"

/*
 * To avoid reversing the string, the caller has to provide a pointer to the
 * end of the string.
 */
char *
itoa(char *s, int n)
{
	*s = '\0';
	if (n == 0)
		*--s = '0';
	for (; n; n /= 10)
		*--s = n % 10 + '0';
	return (s);
}

void *
memset(void *dst, int v, int len)
{
	unsigned char *dst0;

	dst0 = dst;
	while (len--)
		*dst0++ = v;
	return (dst);
}
