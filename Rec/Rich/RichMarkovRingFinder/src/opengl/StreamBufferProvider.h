#ifndef LESTER_STREAMCANVASTOOLS_STREAMBUFFERPROVIDER_H
#define LESTER_STREAMCANVASTOOLS_STREAMBUFFERPROVIDER_H

#include "opengl/BufferProvider.h"
#include "opengl/StreamCanvasTokens.h"

namespace Lester {
  namespace StreamCanvasTools {

    class StreamBufferProvider : public BufferProvider {
    private:
      std::istream * m_dataStream;
    public:
      StreamBufferProvider(std::istream * dataStream ) :
	m_dataStream(dataStream),
	m_thread(0) {
	attachDoubleBuffer();
	attachPrimitiveInputStream();
      };

    private:
      void threadForDealingWithUpdates() {
	std::istream & ciin = *m_dataStream;
	std::cout << "Am dealing with updates!" << std::endl;
	std::string cmd;
	double x1,x2,y1,y2,z1,z2,r,x,y,z;
	while (ciin>>cmd) {
	  std::cout << "I received cmd ["<<cmd<<"]"<<std::endl;
	  try {
	    //if (cmd==Tokens::update) {
	    //  /// Should really remove this line and the notifyOfBufferSuggestion method altogether and create a new class called "ViewSubscriber" to send/receive update suggestions
	    //  //this->notifyOfBufferSuggestion(cmd);
	    //} else
	    if (cmd==Tokens::approve) {
	      /// Should really remove this line and the notifyOfBufferSuggestion method altogether and create a new class called "ViewSubscriber" to send/receive update suggestions
	      doubleBuffer()->approve();
	      std::cout << "erm 1" << __FILE__ << " " << __LINE__ << std::endl;
	      this->notifyOfNewApprovedBufferIn(doubleBuffer());
	      std::cout << "erm 2" << __FILE__ << " " << __LINE__ << std::endl;
	    } else if (cmd==Tokens::approveAndClear) {
	      doubleBuffer()->approveAndClear();
	      this->notifyOfNewApprovedBufferIn(doubleBuffer());	      
	    } else if (cmd==Tokens::colour) {
	      //std::cout << "x1 y1 x2 y2" << std::endl;
	      double red,green,blue;
	      ciin >> red>>green>>blue;
	      doubleBuffer()->add(boost::shared_ptr<Drawable>(new Colour(red,green,blue)));
	    } else if (cmd==Tokens::line2d) {
	      //std::cout << "x1 y1 x2 y2" << std::endl;
	      ciin >> x1>>y1>>x2>>y2;
	      doubleBuffer()->add(boost::shared_ptr<Drawable>(new Line(x1,y1,   x2,y2)));
	    } else if (cmd==Tokens::line3d) {
	      //std::cout << "x1 y1 z1 x2 y2 z2" << std::endl;
	      ciin >> x1>>y1>>z1>>x2>>y2>>z2;
	      doubleBuffer()->add(boost::shared_ptr<Drawable>(new Line(x1,y1,z1,  x2,y2,z2)));
	    } else if (cmd==Tokens::parallelepiped) {
	      ciin >> x>>y>>z;
	      Hep3Vector corner(x,y,z);
	      ciin >> x>>y>>z;
	      Hep3Vector edge1(x,y,z);
	      ciin >> x>>y>>z;
	      Hep3Vector edge2(x,y,z);
	      ciin >> x>>y>>z;
	      Hep3Vector edge3(x,y,z);
	      doubleBuffer()->add(boost::shared_ptr<Drawable>(new Parallelepiped(corner, edge1, edge2, edge3)));
	      
	      /*
		} else if (cmd=="lookAt") {
		std::cout << "x y z" << std::endl;
		ciin >> x>>y>>z;
		//canvas.lookAt(x,y,z);
		} else if (cmd=="moveEyeTo") {
		std::cout << "x y z" << std::endl;
		ciin >> x>>y>>z;
		//canvas.moveEyeTo(x,y,z);
		*/
	      
	    } else if (cmd==Tokens::circle) {
	      //std::cout << "x y r" << std::endl;
	      unsigned int facets;
	      ciin >> x>>y>>r>>facets;
	      doubleBuffer()->add(boost::shared_ptr<Drawable>(new Circle(x,y,r,facets)));
	    } else if (cmd==Tokens::clear) {
	      doubleBuffer()->clear();
	    } else {
	      std::cerr << "Unrecognised cmd [" << cmd << "]" << std::endl; 
	    };
	  } catch (...) {
	    std::cerr << "Syntax error! Try a new command!" << std::endl;
	  };    
	};
      };

    private:
      void attachDoubleBuffer() {
	m_doubleBuffer = boost::shared_ptr<DoubleBuffer>(new DoubleBuffer);
      };
    private:
      void attachPrimitiveInputStream() {
	std::cout << "Flobnbleu jjj" << std::endl;
	m_thread = new boost::thread(boost::bind(&Lester::StreamCanvasTools::StreamBufferProvider::threadForDealingWithUpdates, this));
	std::cout << "Flobnbleu ggg" << std::endl;
      };
    public:
      ~StreamBufferProvider() {
	if (m_thread) {
	  std::cout << "Will now wait for input thread to end!" << std::endl;
	  m_thread->join();
	  delete m_thread;
	  m_thread=0;
	};
      };
    private:
      boost::shared_ptr<DoubleBuffer> doubleBuffer() {
	return m_doubleBuffer;
      };
    private:
      boost::thread * m_thread;
      boost::shared_ptr<DoubleBuffer> m_doubleBuffer; 
    };

  };
};

#endif
