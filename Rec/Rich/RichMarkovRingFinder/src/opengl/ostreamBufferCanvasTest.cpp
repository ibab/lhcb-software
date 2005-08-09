
#include "opengl/OstreamBufferCanvas.h"
#include<iostream>

int main() {

	Lester::StreamCanvasTools::OstreamBufferCanvas canvas(std::cout);
	canvas.line(1,2,3,4);

	return 0;
};
