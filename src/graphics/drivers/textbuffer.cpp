#include "../graphics.h"
#include "drivers.h"

#include <stddef.h>
#include <stdint.h>

using namespace kos;

static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 24;

TextBufferDriver::TextBufferDriver(): r_pos(0), c_pos(0) {
	color = 0;
	buffer = (uint16_t*) 0xB8000;
	for(int i=0; i<VGA_HEIGHT; ++i){
		for(int j=0; j<VGA_WIDTH; ++j){
			size_t ind = i*VGA_WIDTH+j;
			buffer[ind] = getEntry(' ');
		}
	} 
}
 
void TextBufferDriver::setPos(unsigned int x, unsigned int y){
	r_pos = x; c_pos = y;
}

void TextBufferDriver::setFGColor(VGAColor nc){
	color = color & (((1 << 5) - 1) << 4);
	color = color | nc;
}

void TextBufferDriver::setBGColor(VGAColor nc){
	color = color & ((1 << 5) - 1);
	color = color | (nc << 4);
}

void TextBufferDriver::write(char c){
	size_t ind = r_pos*VGA_WIDTH + c_pos;
	buffer[ind] = getEntry(c);
	
	++c_pos;
	if(c_pos == VGA_WIDTH){
		c_pos = 0;
		++r_pos;
		if(r_pos == VGA_HEIGHT){
			r_pos = 0;
		}
	}
	
	
}

uint16_t TextBufferDriver::getEntry(char c){
	uint16_t ch16 = c;
	uint16_t co16 = color;
	return ch16 | (co16 << 8);
}

void TextBufferDriver::draw(){}