// Include files
#include <thread>

// boost
#include <boost/regex.hpp>

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
   using std::to_string;
}

// Factory for instantiation of service objects
DECLARE_SERVICE_FACTORY(Hlt2MonRelaySvc)

//===============================================================================
Hlt2MonRelaySvc::Hlt2MonRelaySvc(const string& name, ISvcLocator* loc)
   : base_class(name, loc),
   m_top{false},
   m_relay{true},
   m_thread{nullptr},
   m_control{nullptr},
   m_context{nullptr}
{
   declareProperty("HostnameRegex", m_hostRegex = "hlt(01|(?<subfarm>[a-f]{1}[0-9]{2})(?<node>[0-9]{2})?)");
   declareProperty("InPort", m_inPort = 31337);
   declareProperty("OutPort", m_inPort = 31338);
   declareProperty("FrontConnection", m_frontCon);
   declareProperty("BackConnection", m_backCon);
   declareProperty("ForceTopRelay", m_forceTop = false);
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
   msg << MSG::INFO << "Initialized ZeroMQ based Hlt2MonRelaySvc" << endmsg;
   sc = serviceLocator()->service("IncidentSvc", m_incidentSvc, true);
   if( !sc.isSuccess() ) {
      msg << MSG::FATAL << "IncidentSvc not found" << endmsg;
      return sc;
   }
   m_incidentSvc->addListener(this, "APP_RUNNING");
   m_incidentSvc->addListener(this, "APP_STOPPED");

   std::string taskName{System::argv()[0]};
   m_lhcb2 = (taskName.substr(0, 4) == "LHCb2");

   char hname[_POSIX_HOST_NAME_MAX];
   if (!gethostname(hname, sizeof(hname))) {
      const string hostname{hname};

      boost::smatch matches;
      boost::match_flag_type flags = boost::match_default;
      boost::regex re_host{m_hostRegex};
      auto result = boost::regex_match(begin(hostname), end(hostname), matches, re_host, flags);

      // This one first as we want to check for forced top/forced on.
      if (matches[1] == "01" || m_forceTop) {
         // top relay
         m_top = true;
      }

      if (!result && (m_frontCon.empty() || m_backCon.empty())) {
         msg << MSG::WARNING << "Not a known host type, and connections not correctly configured, "
             << "relay disabled" << endmsg;
         m_relay = false;
         return sc;
      } else if (!m_frontCon.empty() && !m_backCon.empty()) {
         // If connections were explicitly configured, we're done
         return sc;
      }

      // Deduce connections from hostname.
      if (m_top) {
         m_frontCon = string("tcp://*:") + to_string(m_inPort);
         m_backCon  = string("tcp://*:") + to_string(m_outPort);
      } else if (matches.str("node").empty()) {
         // subfarm relay
         m_frontCon = string("tcp://*:") + to_string(m_inPort);
         m_backCon  = string("tcp://hlt01:") + to_string(m_inPort);
      } else {
         // node relay
         m_frontCon = string("ipc:///tmp/hlt2mon/0");
         m_backCon  = string("tcp://") + matches.str("subfarm") + ":" + to_string(m_inPort);
      }
      return sc;
   } else {
      msg << MSG::FATAL << "Could not determine hostname." << endmsg;
      return StatusCode::FAILURE;
   }
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

   if (!m_lhcb2) return sc;

   if (m_thread) {
      string resume{"RESUME"};
      zmq::message_t msg(resume.size());
      memcpy(static_cast<void*>(msg.data()), resume.c_str(), resume.size());
      m_control->send(msg);
      return sc;
   }

   if (!m_context) {
      m_context = new zmq::context_t{1};
   }

   m_thread = new std::thread([this](void) -> void {
         // Create frontend, backend and control sockets
         zmq::socket_t front{*m_context, ZMQ_XSUB};
         zmq::socket_t back{*m_context, ZMQ_XPUB};
         zmq::socket_t control{*m_context, ZMQ_PAIR};

         MsgStream log(msgSvc(), name());
         //  Bind sockets to TCP ports
         front.bind(m_frontCon.c_str());
         log << MSG::INFO << "Bound frontend to: " << m_frontCon << endmsg;
         if (!m_top) {
            back.connect(m_backCon.c_str());
            log << MSG::INFO << "Connected backend to " << m_backCon << endmsg;
         } else {
            back.connect(m_backCon.c_str());
            log << MSG::INFO << "Bound backend to " << m_backCon << endmsg;
            back.bind(m_backCon.c_str());
         }

         // use inproc for the control.
         control.connect("inproc://control");

         //  Start the queue proxy, which runs until ETERM or "TERMINATE"
         //  received on the control socket
         zmq_proxy_steerable (front, back, nullptr, control);
      });

   m_control = new zmq::socket_t{*m_context, ZMQ_PAIR};
   m_control->bind("inproc://control");

   MsgStream msg(msgSvc(), name());
   msg << MSG::INFO << "Relay started." << endmsg;

   return sc;
}


//===============================================================================
StatusCode Hlt2MonRelaySvc::stop()
{
   if (m_control) {
      string pause{"PAUSE"};
      zmq::message_t msg(pause.size());
      memcpy(static_cast<void*>(msg.data()), pause.c_str(), pause.size());
      m_control->send(msg);
   }
   return StatusCode::SUCCESS;
}

//===============================================================================
StatusCode Hlt2MonRelaySvc::finalize()
{
   MsgStream msg(msgSvc(), "Hlt2MonRelaySvc");

   // terminate the proxy
   if (m_thread) {
      zmq_send(*m_control, "TERMINATE", 9, 0);
      m_thread->join();
      delete m_thread;
      m_thread = nullptr;
      delete m_control;
      m_control = nullptr;
   }
   delete m_context;
   m_context = nullptr;

   if (m_incidentSvc) {
      m_incidentSvc->removeListener(this);
      m_incidentSvc->release();
      m_incidentSvc = 0;
   }
   return Service::finalize();
}
