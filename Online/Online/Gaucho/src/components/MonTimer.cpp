#include "MonTimer.h"

MonTimer::MonTimer(int refreshTime, ISvcLocator* svcLocator, IGauchoMonitorSvc* owner):
  m_refreshTime(refreshTime),
  m_svcLocator(svcLocator),
  m_owner(owner)
{
  StatusCode sc = m_svcLocator->service("MessageSvc", m_msgSvc);
  if ( sc.isFailure() ) {
    std::cout << "Error locating MessageSvc." << std::endl;
    return;
  }

  MsgStream msg(m_msgSvc, "MonTimer");  
  msg << MSG::DEBUG << "MessageSvc located." << endreq;

  m_strand = new boost::asio::strand(m_ioService);

  msg << MSG::DEBUG << "creating boost timer object." << endreq;
  m_asyncTimer = new boost::asio::deadline_timer(m_ioService, boost::posix_time::seconds(m_refreshTime));
  
  msg << MSG::DEBUG << "seting asynchronus wait in timer." << endreq;
  m_asyncTimer->async_wait(m_strand->wrap(boost::bind(&MonTimer::timerHandler, this)));

  std::cout << "creating thread" << std::endl;
  m_thread = new boost::thread(boost::bind(&boost::asio::io_service::run, &m_ioService));
  // if I put this staff then the thread doesn't work in the background
  //m_ioService.run(); 
  //m_thread->join();
}
 
MonTimer::~MonTimer()
{
  MsgStream msg(m_msgSvc, "MonTimer");
  msg << MSG::INFO << "MonTimer destructor." << endreq;
  // m_thread->detach();
   msg << MSG::INFO << "delete m_thread." << endreq;
   delete m_thread; m_thread = 0;
   msg << MSG::INFO << "delete m_asyncTimer." << endreq;
   delete m_asyncTimer; m_asyncTimer = 0;
   msg << MSG::INFO << "delete m_asyncTimer." << endreq;
   delete m_strand; m_strand = 0;
   
}

void MonTimer::timerHandler() {
  MsgStream msg(m_msgSvc, "MonTimer");  
  msg << MSG::DEBUG << "*****************************************" << endreq;
  msg << MSG::DEBUG << "timer Handler called." << endreq;
  // if timerElapsed is true, we're probably waiting for triggers
  // update immediately
  // update will occur start of next event 
  //check if time elapsed much bigger than time of last update
  if (m_owner->getTimerElapsed()){
    msg << MSG::DEBUG << "m_timerElapsed is TRUE then Update all" << endreq;
    m_owner->updateAll(true);
  }
  else {
    msg << MSG::DEBUG << "m_timerElapsed is FALSE then set it to TRUE" << endreq;
    m_owner->setTimerElapsed(true);
  }  
  m_asyncTimer->expires_at(m_asyncTimer->expires_at() + boost::posix_time::seconds(m_refreshTime));
  m_asyncTimer->async_wait(m_strand->wrap(boost::bind(&MonTimer::timerHandler, this)));
  msg << MSG::DEBUG << "*****************************************" << endreq;
}

