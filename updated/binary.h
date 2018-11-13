#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#define BINMODE_WRITE 0
#define BINMODE_APPEND 1
// add two write modes:
// truncate to 0 and append from index 0
// append from current index without truncating
#define pi(x)      printf("%s = %d\n", #x, x);
#define ps(x)      printf("%s = %s\n", #x, x);
#define pc(x)      printf("%s = %c\n", #x, x);
#define pd(x)      printf("%s = %d\n", #x, (int)x);
int bitcount_ = 1;

struct strStream {
    unsigned char *s;
    int bitpos;
};

struct binbuf {
	struct strStream * data;
	size_t length;
	int BINMODE;
	int bitcount;
};

struct strStream * strStream_init(unsigned char * string) {
	struct strStream * str = malloc(sizeof(struct strStream));
	str->s = string;
	str->bitpos = 0;
	return str;
}

void strStream_deinit(struct strStream * * str) {
	(*str)->s = NULL;
	(*str)->bitpos = 0;
	free((*str));
}

struct binbuf binbuf_init(int mode) {
	struct binbuf bin = {0};
	bin.data = strStream_init(NULL);
	bin.BINMODE = mode;
	bin.bitcount = 0;
	return bin;
}

// reads a binary string writing it as literal to a buffer
int read__binary(struct binbuf * bin, char * s) {
    //printf( "INPUT:%s\n", s );
    int newlen = strlen(s);
    int totlen = bin->length + newlen;  // adds number of bits in input string
    
    // realloc new total (rounded up) bytes for new bittotal
    bin->data->s = realloc(bin->data->s, ((bin->length = totlen) + (CHAR_BIT-1)) /CHAR_BIT );
    memset( bin->data->s + ( ( bin->bitcount + 7 ) / 8 ), 0, ( ( bin->length - bin->bitcount + 7 ) / 8 ) );

	int i;
	for( i = 0; i < newlen; i++) {
		bin->data->s[((bin->bitcount + i)>>3)] |= ( s[i] == '1' ? 0x80 : 0 ) >> (7-((bin->bitcount + i  )&7));
	}
	
	// bitcount is really the same as bin->length, but delayed ....
	bin->bitcount += i;
	return 0;
}

// reads a string returning it as a binary string
unsigned char * read__string(struct strStream * strStream, int len) {
	unsigned char * bits = malloc(len+1);
	int i;
	for ( i = 0; i < len; i++) {
	    if( strStream->s[ (strStream->bitpos + i)>>3 ] 
		        & ( 1 << ( ( strStream->bitpos + i ) & 7 ) ) )
		    bits[i] = '1';
		else 
		    bits[i] = '0';
	}
	strStream->bitpos += i;
	bits[i] = 0;
	return bits;
}
