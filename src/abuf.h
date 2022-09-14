#ifndef ABUG_H
#define ABUG_H 1

typedef struct {
	char* buffer; // Actual buffer which stores data
	int len;      // Size of the buffer above
} abuf_t;

#define ABUF_INIT { NULL, 0 }

void abAppend(abuf_t* ab, const char* s, int len);
void abFree(abuf_t* ab);

#endif