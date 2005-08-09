#include <iostream>
#include "opengl/Canvas.h"
#include "boost/thread.hpp"
#include "boost/bind.hpp"
#include "boost/shared_ptr.hpp"
#include "CLHEP/Vector/ThreeVector.h"

#include "opengl/Line.h"
#include "opengl/Circle.h"
#include "opengl/Parallelepiped.h"



namespace Lester {
  namespace StreamCanvasTools {
    



  }; // end of namespace StreamCanvas
}; // end of namespace Lester

#include "opengl/BufferCanvas.h"
#include "opengl/StreamBufferProvider.h"

int main(int argc, char ** argv) {
  /*
   std::cout << "name" << std::endl;
  std::string name;
  std::cin >> name;
  std::cout << "xwid" << std::endl;
  unsigned int xwid;
  std::cin >> xwid;
  std::cout << "ywid" << std::endl;
  unsigned int ywid;
  std::cin >> ywid;
  */
  unsigned int xwid=700,ywid=700;
  std::string name="StreamCanvas";

  std::istream * dataStream = &std::cin;

  boost::shared_ptr<Lester::StreamCanvasTools::BufferProvider> streamBufferProvider = boost::shared_ptr<Lester::StreamCanvasTools::BufferProvider>(new Lester::StreamCanvasTools::StreamBufferProvider(dataStream));

  
  Lester::StreamCanvasTools::BufferCanvas canvas(streamBufferProvider,
		      argc, argv,
		      name,
		      xwid,ywid);

  canvas.startWindowInputUpdateLoop();

  return 0;
};
