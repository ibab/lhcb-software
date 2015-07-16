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
Hlt2MonRelaySvc::Hlt2MonRelaySvc(const string& svcName, ISvcLocator* loc)
   : Hlt2MonBaseSvc(svcName, loc)
{
   declareProperty("HostnameRegex", m_hostRegex = "hlt(01|(?<subfarm>[a-f]{1}[0-9]{2})(?<node>[0-9]{2})?)");
}

//===============================================================================
Hlt2MonRelaySvc::~Hlt2MonRelaySvc()
{

}

//===============================================================================
StatusCode Hlt2MonRelaySvc::initialize()
{
   StatusCode sc = Hlt2MonBaseSvc::initialize();
   if (!sc.isSuccess()) return sc;

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
         warning() << "Not a known host type, and connections not correctly configured, "
             << "relay disabled" << endmsg;
         m_enabled = false;
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
         m_frontCon = string("ipc:///tmp/hlt2MonData_0");
         m_backCon  = string("tcp://hlt") + matches.str("subfarm") + ":" + to_string(m_inPort);
      }
      return sc;
   } else {
      fatal() << "Could not determine hostname." << endmsg;
      return StatusCode::FAILURE;
   }
}

//===============================================================================
void Hlt2MonRelaySvc::function()
{
   // Create frontend, backend and control sockets
   zmq::socket_t front{context(), ZMQ_XSUB};
   zmq::socket_t back{context(), ZMQ_XPUB};
   zmq::socket_t control{context(), ZMQ_SUB};

   //  Bind sockets to TCP ports
   try {
      front.bind(m_frontCon.c_str());
      info() << "Bound frontend to: " << m_frontCon << endmsg;
   } catch (const zmq::error_t&) {
      error() << "Failed to bind frontend to: " << m_frontCon << endmsg;
   }

   if (!m_top) {
      try {
         back.connect(m_backCon.c_str());
         info() << "Connected backend to " << m_backCon << endmsg;
      } catch (const zmq::error_t&) {
         error() << "Failed to connect backend to: " << m_backCon << endmsg;
      }
   } else {
      try {
         back.bind(m_backCon.c_str());
         info() << "Bound backend to " << m_backCon << endmsg;
      } catch (const zmq::error_t&) {
         error() << "Failed to bind backend to: " << m_backCon << endmsg;
      }
   }

   // use inproc for the control.
   control.connect(ctrlCon().c_str());
   control.setsockopt(ZMQ_SUBSCRIBE, "", 0);

   //  Start the queue proxy, which runs until ETERM or "TERMINATE"
   //  received on the control socket
   zmq_proxy_steerable (front, back, nullptr, control);
   int linger = 0;
   front.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
   back.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
   control.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
}
