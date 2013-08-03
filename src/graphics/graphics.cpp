#include "../boot.h"
#include "graphics.h"
#include "drivers/drivers.h"

using namespace kos;

Graphics::Graphics(GraphicsDriver *ndr): drv(ndr) {
	setPos(0, 0);
	setFGColor(COLOR_LIGHT_GREY);
	setBGColor(COLOR_BLACK);
}

void Graphics::write(const char *str){
	for(size_t i=0; i<strlen(str); ++i){
		write(str[i]);
	}
}

void Graphics::write(unsigned int ui, unsigned int val){	
	int rev = 0;
	bool fix = false;
	if((ui%val) == 0){
		ui = ui+1;
		fix = true;
	}
	
	while(true){
		rev+=ui%val;
		ui/=val;
		if(ui == 0) break;
		rev*=val;
	}
	while((!fix && rev!=0) || (fix && rev!=1)){
		char nm = rev%val;
		if(nm < 10) nm = nm + '0';
		else nm = (nm-10) + 'A';
		
		write(nm);
		rev/=val;
	}
	if(fix) write('0');
} 

void Graphics::write(char c){
	if(drv->canWrite()) {
		if(c == '\n') { 
			setPos(r_pos+1, 0);
			return;
		} 
		
		drv->write(c);
	}else {
		/* fill this */
	}
}

void Graphics::draw(){
	if(drv->canDraw()) drv->draw();
	else{
		VGAColor color = getFGColor();
		setFGColor(COLOR_RED);
		//FIXME: use error function
		write("ERROR: Drawing not supported\n");
		setFGColor(color);
	}
}

pair Graphics::getPos(){
	return pair(r_pos, c_pos);
}

void Graphics::setPos(unsigned int r, unsigned int c){
	r_pos = r; c_pos = c;
	drv->setPos(r, c);
}

VGAColor Graphics::getFGColor(){
	return fg_color;
}

void Graphics::setFGColor(VGAColor color){
	fg_color = color;
	drv->setFGColor(color);
}

VGAColor Graphics::getBGColor(){
	return bg_color;
}

void Graphics::setBGColor(VGAColor color){
	bg_color = color;
	drv->setBGColor(color);
}