#include "opengl/Canvas.h"

using namespace Lester;

void doodleOn(Canvas & flum) {
    Colour::kRed().issue();
    flum.clear();
    flum.moveTo(0,0);
    flum.lineTo(0.2,0);
    flum.lineTo(0.2,0.4);
    flum.lineTo(1.0,0.4);
    flum.lineTo(1.0,2.0);
    flum.lineTo(4.0,2.0);
    flum.lineTo(4.0,8.0);
    flum.lineTo(16.0,8.0);
    flum.lineTo(16.0,32.0);
    flum.lineTo(64.0,32.0);
    flum.lineTo(64.0,128.0);
    flum.circle(0,0,0.2);
    flum.circle(0,0,1);
    flum.circle(0,0,10);
    flum.point(-1,-1);
};

void otherDoodleOn(Canvas & flum) {
    Colour::kBlue().issue();
    flum.clear();
    flum.moveTo(0,0);
    flum.lineTo(-0.2,0);
    flum.lineTo(-0.2,0.4);
    flum.lineTo(-1.0,0.4);
    flum.lineTo(-1.0,2.0);
    flum.lineTo(-4.0,2.0);
    flum.lineTo(-4.0,8.0);
    flum.lineTo(-16.0,8.0);
    flum.lineTo(-16.0,32.0);
    flum.lineTo(-64.0,32.0);
    flum.lineTo(-64.0,128.0);
    flum.circle(-0,0,0.2);
    flum.circle(-0,0,1);
    flum.circle(0,0,10);
    flum.point(1,-1);
};

int main(int argc, char*argv[]) {

  try {
  
  Canvas flum1(argc, argv, "flum 1", 200, 200);
  Canvas flum2(argc, argv, "flum 2", 200, 200);
  // flum1.setOrthoProjection(left, bottom, right, top);

  while(true) {
    doodleOn(flum1);
    otherDoodleOn(flum2);
    flum1.update();
    flum2.update();
    //sleep(1);
  };

  return 0;

  } catch (const std::exception & e) {
    std::cerr << "Aborting because ["<<e.what()<<"]"<<std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Aborting due to unknown exception." <<std::endl;
    return 2;
  };
};
