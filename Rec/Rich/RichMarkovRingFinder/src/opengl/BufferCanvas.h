#ifndef LESTER_STREAMCANVASTOOLS_BUFFERCANVAS_H
#define LESTER_STREAMCANVASTOOLS_BUFFERCANVAS_H

#include "opengl/BufferProvider.h"
#include "opengl/Canvas.h"
#include "opengl/ViewProxy.h"
#include "opengl/ActualView.h"

namespace Lester {
  namespace StreamCanvasTools {

    class BufferCanvas : 
      private Lester::Canvas,
      private BufferSubscriber {
    private:
      const bool m_debug;

    public:
      ViewProxy & view() {
	return m_viewProxy;
      };

    private:
      ActualView m_actualView;
      boost::recursive_mutex m_actualView_mutex;
      ViewProxy m_viewProxy;

    private:
      bool m_simpleUpdateRequested;
      mutable boost::recursive_mutex m_simpleUpdateRequested_mutex;

    private:
      bool m_complexUpdateRequested;
      mutable boost::recursive_mutex m_complexUpdateRequested_mutex;

    private:
      bool m_newApprovedBufferWaiting;
      boost::shared_ptr<DoubleBuffer> m_newApprovedBuffer;
      mutable boost::recursive_mutex m_newApprovedBufferWaiting_mutex;

    private:
      ///implements BufferSubscriber's interface:
      virtual void notifyOfAlterationToScratchBufferIn(boost::shared_ptr<DoubleBuffer> buffer) {
	std::cout << "I have recieved notifyOfAlterationToScratchBuffer but I am not properly implemented" << std::endl;
	std::cout << "This is not even a kludge at " << __FILE__ << " " << __LINE__ << std::endl;
      };
      ///implements BufferSubscriber's interface:
      virtual void notifyOfNewApprovedBufferIn(boost::shared_ptr<DoubleBuffer> buffer) {
	boost::recursive_mutex::scoped_lock lock(m_newApprovedBufferWaiting_mutex);
	m_newApprovedBufferWaiting = true;
	m_newApprovedBuffer = buffer; 
 // For Frederic Brochu!
  //this->requestComplexUpdate();
      };
	

    private:
      /// Should really be called "requestSimpleRedraw".  This SCHEDULES a redraw, but does not actually do the redraw itself.
      void requestSimpleUpdate() {
	boost::recursive_mutex::scoped_lock lk(m_simpleUpdateRequested_mutex);
	m_simpleUpdateRequested=true;
      };
    private:
      /// When the user wants to see the next "new frame" i.e. when they suspect that the drawing has advanced a bit since they last looked and they want to see the new state of affairs, they will press a key that will ultimately lead to this method being called.  This SCHEDULES a later check with the information provided by the bufferProvider, but does not actually do the check here and now.
      void requestComplexUpdate() {
	boost::recursive_mutex::scoped_lock lk(m_complexUpdateRequested_mutex);
	m_complexUpdateRequested=true;
      };
    private:
      void flushViewToCanvas() {
	this->view().flushViewTo(this);
      };
      void flushDrawablesToCanvas() {
	if (m_buffer.get()) {
	  std::cout << "Flushing drawables to canvas" << std::endl;
	  m_buffer->flushDrawablesToCanvas(this);
	} else {
	  std::cout << "Couldn't flush drawables to canvas as no buffer has jet been assigned to us!" << __FILE__ << " " << __LINE__ << std::endl;
	};
      };
      void flushAllToCanvas() {
	flushViewToCanvas();
	flushDrawablesToCanvas();
      };
      void updateBufferIfNecessary(const bool requestSimpleRedrawIfUpdateFound=true) {
	boost::recursive_mutex::scoped_lock lk(m_complexUpdateRequested_mutex);
	if (m_complexUpdateRequested) {
	  std::cout << "Someone has requested a complex update" << std::endl;
	  m_complexUpdateRequested=false;

	  // We had better look to see if there are any new buffers waiting!
	  boost::recursive_mutex::scoped_lock lock(m_newApprovedBufferWaiting_mutex);
	  if (m_newApprovedBufferWaiting) {
	    std::cout << "Indeed there are actually new buffers waiting!" << std::endl;
	    assert(m_newApprovedBuffer.get());
	    m_newApprovedBufferWaiting=false;
	    // There is a new buffer waiting .. let's get it
	    m_newApprovedBuffer->putAStableApprovedBufferInto(m_buffer);
	    m_newApprovedBuffer.reset();
	    if (requestSimpleRedrawIfUpdateFound) {
	      this->requestSimpleUpdate();
	    };
	    std::cout << "This was a horrible kludge at " << __FILE__ << " " << __LINE__ << std::endl;
	  } else {
	    std::cout << "Although a complex update was requested, no new buffers were found waiting" << std::endl;
	  };

	} else {
	  if (m_debug) { std::cout << "No-one has requested a complex update" << std::endl; };
	};
      };
      /// Should really be called "doRedrawIfNecessary"
      void doSimpleUpdateIfNecessary() {
	if (m_debug) { std::cout << "Updating buffer if necessary ... " << std::endl; };
	updateBufferIfNecessary();
	if (m_debug) { std::cout << "  ... done updating buffer if necessary" << std::endl; };


	if (m_debug) { std::cout << "Looking for update requests ..." << std::endl; };
	{
	  boost::recursive_mutex::scoped_lock lk(m_simpleUpdateRequested_mutex);
	  if (m_simpleUpdateRequested) {
	    if (m_debug) { std::cout << "Found update request!" << std::endl; };
	    this->clear(); // Clear the canvas
	    {
	      //boost::recursive_mutex::scoped_lock lock(m_alteration_mutex);
	      flushAllToCanvas();
	    };
	    this->update(); // Actually make the canvas flush to the screen!
	    {
	      m_simpleUpdateRequested=false;
	    };
	  } else {
	    if (m_debug) { std::cout << "No update requested" << std::endl; }
	  };
	};
	if (m_debug) { std::cout << " ... finished looking for updates." << std::endl; };
      };
    public:
      BufferCanvas(boost::shared_ptr<BufferProvider> bufferProvider,
		   int argc, char **argv,
		   const std::string & title,
		   const int width, const int height,
		   const int xpos=-1, const int ypos=-1) :
	Lester::Canvas(argc,argv,
		       title,
		       width,height,
		       xpos,ypos),
	m_debug(false),
	m_viewProxy(m_actualView, m_actualView_mutex),
	m_simpleUpdateRequested(false),
	m_complexUpdateRequested(false),
	m_newApprovedBufferWaiting(false),
	m_newApprovedBuffer(),
	m_bufferProvider(bufferProvider) { 
      };


      /// This is the method that really "energises" the canvas .. this is the method that gets it going and responding to key presses, redraw events etc.  Because of X this must be called from the main thread and may not be in a subsidiary thread.
      void startWindowInputUpdateLoop() {
	if (m_bufferProvider.get()) {
	  m_bufferProvider->subscribe(this);
	} else {
	  std::cout << "Not subscribing to a bufferProvider as none was offered during construction " << __FILE__ << " " << __LINE__ << std::endl;
	};
	this->requestSimpleUpdate();
	while(true) {
	  const bool displayManagerSuggestRedraw = process_input();
	  const bool payAttentionToDisplayManager = true;
	  if (payAttentionToDisplayManager && displayManagerSuggestRedraw) {
	    requestSimpleUpdate();
	  };
	  doSimpleUpdateIfNecessary();
	  //std::cout << "Sleeping after process_input" << std::endl;
	  usleep(2000);
	};
      };
    private:
      /// This implements a virtual method from the Canvas base class that lets us handle key-press events.
      virtual void keyPress(const KeySym c) {
	const double deltaTheta=0.2;
	static double deltaThetaMod=1;
	const double logZoomFactor=log(1.4);
	const double translateFactor=0.05;
	std::cout << "Hey -- I detected " << c << std::endl;
	if ('r'==c) {
	  // 'r' == redraw
	  this->requestSimpleUpdate();
	} else if ('n'==c) {
	  // 'n' == next frame
	  this->requestComplexUpdate();
	} else if ('w'==c) {
	  this->view().rotateLeft(+deltaTheta*deltaThetaMod);
	  this->requestSimpleUpdate();
	} else if ('s'==c) {
	  this->view().rotateLeft(-deltaTheta*deltaThetaMod);
	  this->requestSimpleUpdate();
	} else if ('q'==c) {
	  this->view().rotateUp(+deltaTheta*deltaThetaMod);
	  this->requestSimpleUpdate();
	} else if ('a'==c) {
	  this->view().rotateUp(-deltaTheta*deltaThetaMod);
	  this->requestSimpleUpdate();
	} else if ('e'==c) {
	  this->view().rotateView(-deltaTheta*deltaThetaMod);
	  this->requestSimpleUpdate();
	} else if ('d'==c) {
	  this->view().rotateView(+deltaTheta*deltaThetaMod);
	  this->requestSimpleUpdate();
	} else if ('<'==c) {
	  deltaThetaMod/=1.3;
	} else if ('>'==c) {
	  deltaThetaMod*=1.3;
	} else if ('+'==c) {
	  this->view().zoomIn(+logZoomFactor);
	  this->requestSimpleUpdate();
	} else if ('-'==c) {
	  this->view().zoomIn(-logZoomFactor);
	  this->requestSimpleUpdate();
	} else if (65361==c) { //left
	  this->view().moveRight(-translateFactor);
	  this->requestSimpleUpdate();
	} else if (65362==c) { //up
	  this->view().moveUp(translateFactor);
	  this->requestSimpleUpdate();
	} else if (65363==c) { // right
	  this->view().moveRight(translateFactor);
	  this->requestSimpleUpdate();
	} else if (65364==c) { // down
	  this->view().moveUp(-translateFactor);
	  this->requestSimpleUpdate();
	} else if (';'==c) { // forward
	  this->view().moveForward(translateFactor);
	  this->requestSimpleUpdate();
	} else if ('.'==c) { // backward
	  this->view().moveForward(-translateFactor);
	  this->requestSimpleUpdate();
	} else {
	  std::cerr << "Unrecognised KeySym " << static_cast<int>(c) << std::endl;
	};
      };
    public:
      ~BufferCanvas() {
	if (m_bufferProvider.get()) {
	  m_bufferProvider->unsubscribe();
	};
      };
    private:
      boost::shared_ptr<BufferProvider> m_bufferProvider;
      boost::shared_ptr<BufferInterface> m_buffer;
    };

  };
}; // end of namespace Lester


#endif
