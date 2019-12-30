#ifndef __Base64_H
#define __Base64_H

#include <stdbool.h>

extern "C"
{
	bool b58enc(char *b58, size_t *b58sz, const unsigned char *data, size_t binsz);
	bool b58dec(void *bin, size_t *binszp, const char *b58);
}

#endif
