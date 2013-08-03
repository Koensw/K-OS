#ifndef KOS_BOOT_H
#define KOS_BOOT_H

/* 
 * DEFINES ESSENTIAL EARLY BOOTUP FUNCTIONS
 */ 

#include <stddef.h>
#include <stdint.h>

/* required __cxa functions */
extern "C" void __cxa_pure_virtual();

/* essential early functions */
size_t strlen(const char* str);
unsigned int min(unsigned int a, unsigned int b);
    
/* essential early structs */
struct pair{
	pair(int a, int b): first(a), second(b) {}
	int first;
	int second;
};

/* essential debug functions */
void early_panic(int);

#endif