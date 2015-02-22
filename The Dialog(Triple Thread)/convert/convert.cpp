#include "convert.h"

int convert(char *gb2312,unsigned __int16 *unicode)
{
	unsigned __int16 *out=unicode;
	const char *in=gb2312;
	iconv_t converter = iconv_open("UCS-2-INTERNAL","GBK");
	size_t insize,outsize;
	insize=strlen(in);
	outsize=insize+1;
	iconv(converter,&in,&insize,reinterpret_cast<char**>(&out),&outsize);
	iconv_close(converter);
	return 0;
}