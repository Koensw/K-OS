#ifndef KOS_GRAPHICSDRIVERS_H
#define KOS_GRAPHICSDRIVERS_H

#include <stddef.h>
#include <stdint.h>

#include "../graphics.h"
 
namespace kos{ 
	class TextBufferDriver: public GraphicsDriver{
	public:
		TextBufferDriver();
		
		bool canWrite() { return true; }
		void write(char c);
		bool canDraw() { return false; }
		void draw();
		
		void setBGColor(VGAColor color);
		void setFGColor(VGAColor color);
		
		void setPos(unsigned int x, unsigned int y);
	private:
		uint16_t getEntry(char c);
		
		size_t r_pos;
		size_t c_pos;
		uint8_t color;
		uint16_t* buffer;
		
		int height;
		int width;
	};
}

#endif