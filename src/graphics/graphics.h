#ifndef KOS_GRAPHICS_H
#define KOS_GRAPHICS_H

#include "../boot.h"
#include "maps/color.h"

namespace kos{
	struct Mode{
		int height;
		int width;
	};
	
	
	class GraphicsDriver{
	public:
		virtual bool canWrite() = 0;
		virtual void write(char c) = 0;
		virtual bool canDraw() = 0;
		virtual void draw() = 0;
		
		virtual void setBGColor(VGAColor c) = 0;
		virtual void setFGColor(VGAColor c) = 0;
		
		virtual void setPos(unsigned int x, unsigned int y) = 0;
	};
	 
	class Graphics{
	public:
		Graphics(GraphicsDriver*);
		~Graphics() {}
		
		/*Mode getMode();
		void setMode(int);
		
		int countMods();
		void probeMods(Mode*);
		
		void switchd(GraphicsDriver *);
		
		void update();*/
		
		void write(const char *);
		void write(int in, unsigned int val = 10) { write(static_cast<unsigned int>(in), val); }
		void write(unsigned int, unsigned int val = 10);
		void write(char);
		void draw();
		VGAColor getBGColor();
		void setFGColor(VGAColor color);
		VGAColor getFGColor();
		void setBGColor(VGAColor color);
		
		pair getPos();
		void setPos(unsigned int, unsigned int);
	private:
		GraphicsDriver *drv;
		VGAColor fg_color;
		VGAColor bg_color;
		
		unsigned int r_pos;
		unsigned int c_pos;
	};
}

#endif