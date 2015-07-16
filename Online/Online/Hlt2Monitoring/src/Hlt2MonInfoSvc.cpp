// Include files
#include <thread>
#include <array>

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
#include <Hlt2Monitoring/RunInfo.h>
#include <Hlt2Monitoring/Serialize.h>

// Local
#include "Hlt2MonInfoSvc.h"

namespace {
   using std::array;
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

   // Create frontend, backend and control sockets
   zmq::socket_t data{context(), ZMQ_SUB};
   data.connect(m_frontCon.c_str());
   data.setsockopt(ZMQ_SUBSCRIBE, "", 0);
   info() << "Connected data input socket to: " << m_frontCon << endmsg;

   zmq::socket_t inf{context(), ZMQ_REP};
   inf.bind(m_backCon.c_str());
   info() << "Bound info service socket to: " << m_backCon << endmsg;

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
      zmq::poll (&items [0], 3, -1);

      if (items[0].revents & ZMQ_POLLIN) {
         // Control messages
         auto cmd = receiveString(control);
         if (cmd == "TERMINATE") {
            int linger = 0;
            data.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
            inf.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            control.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            break;
         } else if (cmd == "PAUSE") {
            paused = true;
         } else if (cmd == "RESUME") {
            paused = false;
         }
      }
      if (!paused && (items[1].revents & ZMQ_POLLIN)) {
         auto type = receiveString(data);
         debug() << "New info message: " << type << endmsg;

         if (type == Monitoring::s_HistoInfo) {
            receiveHistoInfo(data);
         } else if (type == Monitoring::s_RunInfo) {
            receiveRunInfo(data);
         } else {
            warning() << "Unknown type of info message received: " << type << endmsg;
         }
      }
      if (!paused && (items[2].revents & ZMQ_POLLIN)) {
         // Requests for info
         // what type
         auto type = receiveString(inf);
         debug() << "New info request: " << type << endmsg;

         if (type == Monitoring::s_HistoInfo) {
            histoInfoRequest(inf);
         } else if (type == Monitoring::s_RunInfo) {
            runInfoRequest(inf);
         } else {
            warning() << "Unknown type of info request received: " << type << endmsg;
         }
      }
   }
}

//===============================================================================
bool Hlt2MonInfoSvc::receiveHistoInfo(zmq::socket_t& data) const
{
   auto key = receiveRunAndId(data);
   Monitoring::RunNumber run = key.first;

   zmq::message_t message;
   if (m_histograms.count(key) || m_rates.count(key)) {
      // We've already seen this one, only need to receive the
      // rest of the messages
      while (true) {
         data.recv(&message);
         if (!message.more())
            break;
         message.rebuild();
      }
   } else {

      // Run start time
      message.rebuild();
      data.recv(&message);
      int start{0};
      memcpy(&start, message.data(), sizeof(start));
      m_startTimes[run] = start;

      // What type
      std::string type = receiveString(data);

      debug() << "New histogram: " << start << " " << type;

      // Last part, the content.
      if (type == Monitoring::s_Rate) {
         m_rates[key] = receiveString(data);
         debug() << " " << m_rates[key] << endmsg;
      } else if (type == Monitoring::s_Histo1D) {
         // Deserialize Histo1DDef
         Gaudi::Histo1DDef def;
         std::stringstream ss{receiveString(data)};
         {
            boost::archive::text_iarchive ia{ss};
            ia >> def;
         }
         // Add to internal store
         m_histograms[key] = def;
         debug() << " " << def.title() << endmsg;
      } else {
         debug() << endmsg;
      }
   }
   // FIXME: improve error handling
   return true;
}

//===============================================================================
bool Hlt2MonInfoSvc::receiveRunInfo(zmq::socket_t& data) const
{
   // Deserialize RunInfo
   Monitoring::RunInfo runInfo;
   std::stringstream ss{receiveString(data)};
   {
      boost::archive::text_iarchive ia{ss};
      ia >> runInfo;
   }

   // Add to internal store
   debug() << "New run info for run: " << runInfo.run << endmsg;
   m_runInfo[runInfo.run] = runInfo;
   return true;
}

//===============================================================================
bool Hlt2MonInfoSvc::histoInfoRequest(zmq::socket_t& inf) const
{
   // What run and ID requested?
   Monitoring::RunNumber run{0};
   Monitoring::HistId id{0};
   auto key = receiveRunAndId(inf);
   std::tie(run, id) = key;

   // Prepare reply
   string known;
   string start;
   string type;
   string reply;

   if (m_histograms.count(key)) {
      known = Monitoring::s_Known;
      type = Monitoring::s_Histo1D;
      // Serialize and send Histo1DDef
      std::stringstream output;
      {
         boost::archive::text_oarchive oa{output};
         oa << m_histograms[key];
      }
      reply = output.str();
      start = to_string(m_startTimes[run]);
   } else if (m_rates.count(key)) {
      known = Monitoring::s_Known;
      type = Monitoring::s_Rate;
      reply = m_rates[key];
      start = to_string(m_startTimes[run]);
   } else {
      known = Monitoring::s_Unknown;
   }

   debug() << "Sending info reply: " << run << " " << id << " |";
   // Send reply
   array<string, 4> rep = {known, start, type, reply};
   for (auto it = begin(rep), last = end(rep); it != last; ++it) {
      debug() << *it << "|";
      sendString(inf, *it, (it != (last -1) ? ZMQ_SNDMORE : 0));
   }
   debug() << endmsg;
   return true;
}

//===============================================================================
bool Hlt2MonInfoSvc::runInfoRequest(zmq::socket_t& inf) const
{
   // Incoming IDs
   zmq::message_t message;
   inf.recv(&message);
   Monitoring::RunNumber run{0};
   memcpy(&run, message.data(), sizeof(run));

   string known{Monitoring::s_Unknown}, reply;
   auto it = m_runInfo.find(run);
   if (it != end(m_runInfo)) {
      known = Monitoring::s_Known;

      // Serialize RunInfo
      std::stringstream ss;
      {
         boost::archive::text_oarchive oa{ss};
         oa << it->second;
      }
      reply = ss.str();
   }

   debug() << "Sending info reply: |";
   array<string, 2> rep = {known, reply};
   for (auto it = begin(rep), last = end(rep); it != last; ++it) {
      debug() << *it << "|";
      sendString(inf, *it, (it != (last - 1) ? ZMQ_SNDMORE : 0));
   }
   debug() << endmsg;
   return true;
}
