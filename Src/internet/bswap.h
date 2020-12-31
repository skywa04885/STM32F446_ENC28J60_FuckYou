#ifndef _SRC_INTERNET_BSAP_H
#define _SRC_INTERNET_BSWAP_H

#define BSWAP16(A) ((((A) & 0x00FF) << 8) | (((A) & 0xFF00) >> 8))
#define BSWAP32(A) (\
	(((A) & 0x000000FF) << 24) | 	\
	(((A) & 0x0000FF00) << 8) | 	\
	(((A) & 0x00FF0000) >> 8) | 	\
	(((A) & 0xFF000000) >> 24) 		\
)

#endif
