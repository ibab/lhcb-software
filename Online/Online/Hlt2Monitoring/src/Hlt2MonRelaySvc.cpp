// Include files
#include <thread>

// zeromq
#include "zmq/zmq.hpp"

// Gaudi
#include <GaudiKernel/IIncidentSvc.h>
#include <GaudiKernel/IAlgorithm.h>
#include <GaudiKernel/IAlgTool.h>
#include <GaudiKernel/SvcFactory.h>
#include <GaudiKernel/MsgStream.h>

#include "Hlt2MonRelaySvc.h"

namespace {
   using std::string;

}
// Factory for instantiation of service objects
DECLARE_SERVICE_FACTORY(Hlt2MonRelaySvc)

//===============================================================================
Hlt2MonRelaySvc::Hlt2MonRelaySvc(const string& name, ISvcLocator* loc)
   : base_class(name, loc),
     m_thread{nullptr},
     m_control{nullptr},
     m_context{nullptr}
{
}

//===============================================================================
Hlt2MonRelaySvc::~Hlt2MonRelaySvc()
{

}

//===============================================================================
StatusCode Hlt2MonRelaySvc::initialize()
{
   StatusCode sc = Service::initialize();

   MsgStream msg(msgSvc(), name());
   sc = serviceLocator()->service("IncidentSvc", m_incidentSvc, true);
   if( !sc.isSuccess() ) {
      msg << MSG::FATAL << "IncidentSvc not found" << endmsg;
      return sc;
   }
   m_incidentSvc->addListener(this, "APP_RUNNING");
   m_incidentSvc->addListener(this, "APP_STOPPED");
   return sc;
}

//===============================================================================
void Hlt2MonRelaySvc::handle(const Incident& /* incident */)
{

}

//===============================================================================
StatusCode Hlt2MonRelaySvc::start()
{
   auto sc = Service::start();
   if (!sc.isSuccess()) return sc;

   if (!m_control) {
      m_control = new zmq::socket_t{*m_context, ZMQ_PAIR};
      m_control->bind("inproc://control");
   }

   if (m_thread) return sc;

   m_thread = new std::thread([this] {
      // Create frontend, backend and control sockets
      zmq::socket_t front{*m_context, ZMQ_XSUB};
      zmq::socket_t back{*m_context, ZMQ_XPUB};
      zmq::socket_t control{*m_context, ZMQ_PAIR};

      //  Bind sockets to TCP ports
      front.bind(m_frontConnection.c_str());
      back.bind(m_backConnection.c_str());

      // use inproc for the control.
      control.connect("inproc://control");

      //  Start the queue proxy, which runs until ETERM or "TERMINATE"
      //  received on the control socket
      zmq_proxy_steerable (front, back, nullptr, control);
   });

   return sc;
}


//===============================================================================
StatusCode Hlt2MonRelaySvc::stop()
{
   // terminate the proxy
   zmq_send(*m_control, "TERMINATE", 9, 0);
   m_thread->join();
   delete m_thread;
   m_thread = 0;
   delete m_control;
   m_control = nullptr;
   delete m_context;
   m_context = nullptr;
   return StatusCode::SUCCESS;
}

//===============================================================================
StatusCode Hlt2MonRelaySvc::finalize()
{
   MsgStream msg(msgSvc(), "Hlt2MonRelaySvc");

   if (m_incidentSvc) {
      m_incidentSvc->removeListener(this);
      m_incidentSvc->release();
      m_incidentSvc = 0;
   }
   return Service::finalize();
}
