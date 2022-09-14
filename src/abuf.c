#include <stdlib.h>
#include <string.h>
#include "abuf.h"

void abAppend(abuf_t* ab, const char* s, int len) {
	char* new = realloc(ab->buffer, ab->len + len);

	if (new == NULL) return;
	memcpy(&new[ab->len], s, len);
	ab->buffer = new;
	ab->len += len;
}

void abFree(abuf_t* ab) {
	free(ab->buffer);
}
