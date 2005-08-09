#ifndef LESTER_STREAMCANVASTOOLS_BUFFERPROVIDER_H
#define LESTER_STREAMCANVASTOOLS_BUFFERPROVIDER_H

#include "opengl/Buffer.h"
#include "opengl/BufferSubscriber.h"

namespace Lester {
  namespace StreamCanvasTools {

    class BufferProvider {
    private:
      BufferSubscriber * m_subscriber;
    protected:
      BufferSubscriber * subscriber() {
	return m_subscriber;
      };
    public:
      BufferProvider() {
      };
    public:
      void subscribe(BufferSubscriber * subscriber) {
	m_subscriber=subscriber;
      };
      void unsubscribe() {
	m_subscriber=0;
      };
    private:
      bool haveASubscriber() const {
	return m_subscriber;
      };
    protected:
      void notifyOfAlterationToScratchBufferIn(boost::shared_ptr<DoubleBuffer> buffer) {
	if (haveASubscriber()) {
	  m_subscriber->notifyOfAlterationToScratchBufferIn(buffer);
	} else {
	  std::cout << "Not yet subscribed, so notifyOfAlterationToScratchBufferIn not notified" << std::endl;
	};
      };
      void notifyOfNewApprovedBufferIn(boost::shared_ptr<DoubleBuffer> buffer) {
	while (!haveASubscriber()) {
	  std::cout << "Waiting for buffer subscription to be made!" << std::endl;
	  sleep(1);
	};
	if (haveASubscriber()) {
	  m_subscriber->notifyOfNewApprovedBufferIn(buffer);
	} else {
	  std::cout << "Not yet subscribed, so notificationOfNewApproval not notified" << std::endl;
	};
      };
    };
  };
};

#endif
