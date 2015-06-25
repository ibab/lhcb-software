// Include files
#include <thread>

// boost
#include <boost/regex.hpp>
#include <boost/unordered_map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

// zeromq
#include "zmq/zmq.hpp"
#include <ZeroMQ/IZeroMQSvc.h>

// Gaudi
#include <GaudiKernel/IIncidentSvc.h>
#include <GaudiKernel/IAlgorithm.h>
#include <GaudiKernel/IAlgTool.h>
#include <GaudiKernel/SvcFactory.h>
#include <GaudiKernel/MsgStream.h>

// Hlt2Monitoring
#include <Hlt2Monitoring/Common.h>
#include <Hlt2Monitoring/Serialize.h>

// Local
#include "Hlt2MonInfoSvc.h"

namespace {
   using std::string;
   using std::to_string;
}

// Factory for instantiation of service objects
DECLARE_SERVICE_FACTORY(Hlt2MonInfoSvc)

//===============================================================================
Hlt2MonInfoSvc::Hlt2MonInfoSvc(const string& name, ISvcLocator* loc)
   : Hlt2MonBaseSvc(name, loc)
{
   declareProperty("HostnameRegex", m_hostRegex = "hlt(01|(?<subfarm>[a-f]{1}[0-9]{2})(?<node>[0-9]{2})?)");
}

//===============================================================================
Hlt2MonInfoSvc::~Hlt2MonInfoSvc()
{

}

//===============================================================================
StatusCode Hlt2MonInfoSvc::initialize()
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
         m_frontCon = string("ipc:///tmp/hlt2MonInfo_0");
         m_backCon  = string("tcp://") + matches.str("subfarm") + ":" + to_string(m_inPort);
      }
      return sc;
   } else {
      fatal() << "Could not determine hostname." << endmsg;
      return StatusCode::FAILURE;
   }
}

//===============================================================================
void Hlt2MonInfoSvc::function()
{
   m_top ? runTop() : runRelay();
}

//===============================================================================
void Hlt2MonInfoSvc::runRelay()
{
   // Create frontend, backend and control sockets
   zmq::socket_t front{context(), ZMQ_PULL};
   zmq::socket_t back{context(), ZMQ_PUSH};
   zmq::socket_t control{context(), ZMQ_SUB};

   //  Bind sockets to TCP ports
   front.bind(m_frontCon.c_str());
   info() << "Bound frontend to: " << m_frontCon << endmsg;
   back.connect(m_backCon.c_str());
   info() << "Connected backend to " << m_backCon << endmsg;

   // use inproc for the control.
   control.connect(ctrlCon().c_str());
   control.setsockopt(ZMQ_SUBSCRIBE, "", 0);

   //  Start the queue proxy, which runs until ETERM or "TERMINATE"
   //  received on the control socket
   zmq_proxy_steerable (front, back, nullptr, control);

   // We're done, set linger to 0 to get a clean exit.
   int linger = 0;
   front.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
   back.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
   control.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
}

//===============================================================================
void Hlt2MonInfoSvc::runTop()
{
   // Create frontend, backend and control sockets
   zmq::socket_t data{context(), ZMQ_PULL};
   data.bind(m_frontCon.c_str());
   info() << "Connected frontend to: " << m_frontCon << endmsg;

   zmq::socket_t inf{context(), ZMQ_REP};
   inf.bind(m_backCon.c_str());
   info() << "Bound backend to: " << m_backCon << endmsg;

   zmq::socket_t control{context(), ZMQ_SUB};
   control.connect(ctrlCon().c_str());
   control.setsockopt(ZMQ_SUBSCRIBE, "", 0);

   //  Initialize poll set
   zmq::pollitem_t items [] = {
      { control, 0, ZMQ_POLLIN, 0 },
      { data, 0, ZMQ_POLLIN, 0 },
      { inf, 0, ZMQ_POLLIN, 0 }
   };

   bool paused = false;

   while (true) {
      //  Process messages from all sockets
      zmq::message_t message;

      using key_t = std::pair<Monitoring::RunNumber, Monitoring::HistId>;
      boost::unordered_map<key_t, Gaudi::Histo1DDef> histograms;
      boost::unordered_map<key_t, string> rates;

      zmq::poll (&items [0], 3, -1);

      if (items[0].revents & ZMQ_POLLIN) {
         // Control messages
         control.recv(&message);
         if (message.size() == 9 && memcmp(message.data(), "TERMINATE", 9) == 0) {
            int linger = 0;
            data.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
            inf.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            control.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            break;
         } else if (message.size() == 5 && memcmp(message.data(), "PAUSE", 5) == 0) {
            paused = true;
         } else if (message.size() == 6 && memcmp(message.data(), "RESUME", 6) == 0) {
            paused = false;
         }
      }
      if (!paused && (items[1].revents & ZMQ_POLLIN)) {
         // Incoming IDs
         data.recv(&message);
         Monitoring::HistId id{0};
         memcpy(&id, message.data(), sizeof(id));
         // RunNumber
         message.rebuild();
         data.recv(&message);
         Monitoring::RunNumber run{0};
         memcpy(&run, message.data(), sizeof(run));

         key_t key{run, id};
         if (histograms.count(key) || rates.count(key)) {
            // We've already seen this one, only need to receive the
            // rest of the messages
            while (message.more()) {
               message.rebuild();
               data.recv(&message);
            }
         } else {
            // What type
            message.rebuild();
            data.recv(&message);
            std::string type{static_cast<char*>(message.data())};

            // Last part, the content.
            message.rebuild();
            data.recv(&message);
            if (type == Monitoring::s_Rate) {
               std::string title{static_cast<char*>(message.data())};
               rates[key] = title;
            } else if (type == Monitoring::s_Histo1D) {
               // Deserialize Histo1DDef
               std::stringstream ss{static_cast<char*>(message.data())};
               boost::archive::text_iarchive ia{ss};
               Gaudi::Histo1DDef def;
               ia >> def;
               // Add to internal store
               histograms[key] = def;
            }
         }
      }
      if (!paused && (items[2].revents & ZMQ_POLLIN)) {
         // Requests for info
         // What ID requested?
         inf.recv(&message);
         Monitoring::HistId id{0};
         memcpy(&id, message.data(), sizeof(id));
         // RunNumber
         message.rebuild();
         inf.recv(&message);
         Monitoring::RunNumber run{0};
         memcpy(&run, message.data(), sizeof(run));

         key_t key{run, id};

         // Prepare reply
         string known;
         string type;
         string reply;

         if (histograms.count(key)) {
            known = Monitoring::s_Known;
            type = Monitoring::s_Histo1D;
            // Serialize and send Histo1DDef
            std::stringstream output;
            boost::archive::text_oarchive oa{output};
            oa << histograms[key];
            reply = output.str();
         } else if (rates.count(key)) {
            known = Monitoring::s_Known;
            type = Monitoring::s_Rate;
            reply = rates[key];
         } else {
            known = Monitoring::s_Unknown;
         }

         // Send reply
         for (const auto& s : {known, type, reply}) {
            message.rebuild(s.size());
            memcpy(static_cast<void*>(message.data()), s.c_str(), s.size());
            inf.send(message, (s != reply ? ZMQ_SNDMORE : 0));
         }
      }
   }
}
