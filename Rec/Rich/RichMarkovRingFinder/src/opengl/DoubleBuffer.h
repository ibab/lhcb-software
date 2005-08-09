
#ifndef LESTER_STREAMCANVASTOOLS_DOUBLEBUFFER_H
#define LESTER_STREAMCANVASTOOLS_DOUBLEBUFFER_H

#include "opengl/Canvas.h"
#include "opengl/DoubleBufferInterface.h"
#include "opengl/Buffer.h"

namespace Lester {
  namespace StreamCanvasTools {

    class DoubleBuffer : virtual public DoubleBufferInterface {
      // Implement the DoubleBufferInterface:
    public:
      virtual void clear() {
        boost::recursive_mutex::scoped_lock lock(m_scratchBuffer_recursive_mutex);
	scratchBuffer()->clear();
      };
      virtual void add(boost::shared_ptr<Drawable> d) {
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
	boost::recursive_mutex::scoped_lock lock(m_scratchBuffer_recursive_mutex);
        scratchBuffer()->add(d);
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
      };
      virtual void flushDrawablesToCanvas(Lester::Canvas * canvas) const {
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
	// Apply locks from big to small to protect agains somone else trying to get locks 2 and 3 in reverse order leading to deadlock.
        boost::recursive_mutex::scoped_lock lock(m_scratchBuffer_recursive_mutex);
	scratchBuffer()->flushDrawablesToCanvas(canvas);
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
      };
      virtual void approveAndClear() {
	boost::recursive_mutex::scoped_lock lock(m_scratchBuffer_recursive_mutex);
	swapScratchBuffers();
	// If we are going to clear, we don't need to copy the old buffer into the new one!
	(*m_currentScratchBuffer)->clear();
      };
      virtual void approve() {
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
        boost::recursive_mutex::scoped_lock lock(m_scratchBuffer_recursive_mutex);
	//std::cout << "I was approved!" << std::endl;
	swapScratchBuffers();
	// approve is not supposed to "damage" the buffer, so need to copy old scratch buffer to new scratch buffer
	*(m_currentScratchBuffer->get()) = *(m_otherScratchBuffer->get());
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
      };
      DoubleBuffer() :
	m_scratchBuffer1(boost::shared_ptr<Buffer>(new Buffer)),
	m_scratchBuffer2(boost::shared_ptr<Buffer>(new Buffer)),
	m_currentScratchBuffer(&m_scratchBuffer1),
	m_otherScratchBuffer  (&m_scratchBuffer2) {
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
      };
      virtual void putAStableApprovedBufferInto(boost::shared_ptr<BufferInterface> & buffer) {
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
	std::cout << "Dumb copy method used at the moment!" << std::endl;
        boost::recursive_mutex::scoped_lock lock(m_scratchBuffer_recursive_mutex);
	Buffer & currentApprovedState = *(approvedBuffer().get());
	buffer = boost::shared_ptr<BufferInterface>(new Buffer(currentApprovedState));
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
      };
    private:
      void swapScratchBuffers() {
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
	boost::recursive_mutex::scoped_lock lock(m_scratchBuffer_recursive_mutex);
	boost::shared_ptr<Buffer> * tmp = m_currentScratchBuffer;
	m_currentScratchBuffer = m_otherScratchBuffer;
	m_otherScratchBuffer = tmp;
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
      };

    private:
      mutable boost::recursive_mutex m_scratchBuffer_recursive_mutex;
      boost::shared_ptr<Buffer> m_scratchBuffer1;
      boost::shared_ptr<Buffer> m_scratchBuffer2;
      boost::shared_ptr<Buffer> * m_currentScratchBuffer;
      boost::shared_ptr<Buffer> * m_otherScratchBuffer;

      //private:
      //  mutable boost::recursive_mutex m_approvedBuffer_recursive_mutex;
      //  boost::shared_ptr<Buffer> m_approvedBuffer;

    private:
      /// This method (and all similar ones) should really return an object of type boost::shared_ptr<BufferInterface>.  This should be easily fixed .. it is just a matter of tracking down all the instances ...
      boost::shared_ptr<Buffer> scratchBuffer() {
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
        boost::recursive_mutex::scoped_lock lock(m_scratchBuffer_recursive_mutex);
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
	return *m_currentScratchBuffer;
      };
      boost::shared_ptr<const Buffer> scratchBuffer() const {
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
        boost::recursive_mutex::scoped_lock lock(m_scratchBuffer_recursive_mutex);
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
	return *m_currentScratchBuffer;
      };
      boost::shared_ptr<Buffer> approvedBuffer() {
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
        boost::recursive_mutex::scoped_lock lock(m_scratchBuffer_recursive_mutex);
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
	return *m_otherScratchBuffer;
      };
      boost::shared_ptr<const Buffer> approvedBuffer() const {
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
        boost::recursive_mutex::scoped_lock lock(m_scratchBuffer_recursive_mutex);
	//std::cout <<" --- " << __FILE__ << " " << __LINE__ << std::endl;
	return *m_otherScratchBuffer;
      };
    };
    
  };
};

#endif
