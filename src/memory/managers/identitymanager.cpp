#include "managers.h"
#include "../memory.h"
#include "../../boot.h"

using namespace kos;

void *IdentityManager::virt(void *ptr){
	return ptr;
}	

void *IdentityManager::phys(void *ptr){
	return ptr;
}

void *IdentityManager::search(unsigned int sz, void *ptr){
	(void) sz;
	
	if(sz == 1) return ptr;
	else return 0;
}