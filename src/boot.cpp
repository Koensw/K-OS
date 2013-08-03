#include "boot.h"

extern "C" void __cxa_pure_virtual() { while (1); }

size_t strlen(const char* str)
{
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
} 

unsigned int min(unsigned int a, unsigned int b){
	if(a < b) return a;
	else return b;
}

void early_panic(int status){
	//setup register error code and triple fault
	asm ("movl %0, %%eax;"
		  "cli;"
		  "hlt;"
     :
     : "r" (status)         
     :"%eax"         
     );
}