#ifndef UrlencodeH
#define UrlencodeH

/**
 * url encode, except: [-_.!~*'();/?:@&=+$,#]
 *
 * @param char* buf, output buffer.
 * @param size_t blen, length of buf.
 * @param const char* s, the url to be encoded.
 *
 * @return char*, returns first address of *buf.
 *
 * @note buf MUST BE different from s.
 */
char* urlencode(char* buf, size_t blen, const char* s);

/**
 * url encode, except: [_-.]
 * @see urlencode()
 */
char* urlencode_all(char* buf, size_t blen, const char* s);

/**
 * url decode
 * @see urlencode()
 */
char* urldecode(char* buf, size_t blen, const char* s);

#endif
