#include <stddef.h>
#include <stdint.h>

#include "../external/multiboot.h"
#include "boot.h"

#include "memory/memory.h"
#include "memory/drivers/drivers.h"
#include "memory/managers/managers.h"
#include "memory/maps/memmap.h"

#include "graphics/drivers/drivers.h"
#include "graphics/graphics.h"
 
/* early panic signals
 * 1 = invalid way to boot
 * 2 = needed memory structs at unsafe locations
 * 3 = no memory left
 */
 
extern unsigned int KERNEL_BEGIN;
extern unsigned int KERNEL_END;
using namespace kos;

extern "C" /* Use C linkage for kernel_main. */
void kinit (int magic, multiboot_info *mbt)
{	
	/* ACCESS OUTSIDE OWN RAM IS DISALLOWED */
	if(magic != 0x2BADB002 || (mbt->flags & (1 << 6)) == false){
		//this is not multiboot or multiboot without memory, this will not work!
		early_panic(0xDEAD0001);
	}	
	
	//initialize modules with default drivers
	TextBufferDriver gdrv;
	Graphics grph(&gdrv);
	
	//read multiboot memory map
	MemoryMap *ptr, *mmap;
	mmap = reinterpret_cast<MemoryMap*>(mbt->mmap_addr);
	
	if(&KERNEL_BEGIN < reinterpret_cast<unsigned int*>(mmap)) {
		//memory struct at unsafe location
		early_panic(0xDEAD0002);
	}
	
	MemoryMap *save = 0;
	ptr = 0;
	bool first = true;
	while(true){
		if(!first) mmap = reinterpret_cast<MemoryMap*>(reinterpret_cast<unsigned int>(mmap) + reinterpret_cast<unsigned int>(mmap->next) + sizeof(unsigned int));
		first = false;
		
		if(reinterpret_cast<unsigned int>(mmap) == mbt->mmap_addr + mbt->mmap_length) break;
		
		//only ram and not to big mem (tmp)
		if(mmap->type == 1 && !(mmap->base_high || mmap->length_high) && reinterpret_cast<unsigned int>(&KERNEL_END) < mmap->base_low + mmap->length_low){
			//adjust memory to be above our kernel
			if(mmap->base_low < reinterpret_cast<unsigned int>(&KERNEL_END)){
				mmap->length_low = (mmap->base_low + mmap->length_low) - reinterpret_cast<unsigned int>(&KERNEL_END);
				mmap->base_low = reinterpret_cast<unsigned int>(&KERNEL_END)+sizeof(unsigned int);
			}
			
			if(save) save->next = reinterpret_cast<unsigned int*>(mmap);
			else ptr = mmap;
			save = mmap;
			
		}
	}
	if(save) save->next = 0;
	
	if(ptr == 0){
		//no memory left to work with
		early_panic(0xDEAD003);
	}
	
	//setup memory
	StackDriver stk(ptr);
	IdentifyManager mgn;
	Memory mem(&stk, &mgn);
	
	mem.print(grph);
	int *ft = static_cast<int*>(mem.alloc(4070));
	grph.write(reinterpret_cast<unsigned int>(ft), 16);
	grph.write('\n');
	mem.print(grph);
	int *chk = static_cast<int*>(mem.alloc(4060));
	grph.write(reinterpret_cast<unsigned int>(chk), 16);
	grph.write('\n');
	//if(chk == 0) grph.write("OOM\n");
	mem.print(grph);
	mem.dealloc(ft);
	mem.print(grph);
	mem.dealloc(chk);
	mem.print(grph);
	
	grph.write("SUCCESSFULL");
	return;
}