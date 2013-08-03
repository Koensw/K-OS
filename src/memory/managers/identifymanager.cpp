#include "managers.h"
#include "../memory.h"
#include "../../boot.h"

using namespace kos;

void *IdentifyManager::virt(void *ptr){
	return ptr;
}	

void *IdentifyManager::phys(void *ptr){
	return ptr;
}