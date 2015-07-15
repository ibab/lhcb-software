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

#include "Hlt2MonBaseSvc.h"

namespace {
   using std::string;
   using std::to_string;
   using std::make_pair;
}

//===============================================================================
Hlt2MonBaseSvc::Hlt2MonBaseSvc(const string& name, ISvcLocator* loc)
 : base_class(name, loc),
   m_top{false},
   m_enabled{true},
   m_incidentSvc{nullptr},
   m_thread{nullptr},
   m_control{nullptr},
   m_zmqSvc{nullptr}

{
   declareProperty("InPort", m_inPort = 31337);
   declareProperty("OutPort", m_outPort = 31338);
   declareProperty("FrontConnection", m_frontCon);
   declareProperty("BackConnection", m_backCon);
   declareProperty("ControlConnection", m_ctrlCon = string{"inproc://"} + name + "Control");
   declareProperty("ForceTop", m_forceTop = false);
   declareProperty("PartitionName", m_partition);
   declareProperty("RunInPartitions", m_partitions = {"LHCb2"});
}

//===============================================================================
Hlt2MonBaseSvc::~Hlt2MonBaseSvc()
{

}

//===============================================================================
StatusCode Hlt2MonBaseSvc::initialize()
{
   StatusCode sc = Service::initialize();

   info() << "Initialized ZeroMQ based service: " << name() << endmsg;
   sc = serviceLocator()->service("IncidentSvc", m_incidentSvc, true);
   if( !sc.isSuccess() ) {
      fatal() << "IncidentSvc not found" << endmsg;
      return sc;
   }
   m_incidentSvc->addListener(this, "APP_RUNNING");
   m_incidentSvc->addListener(this, "APP_STOPPED");

   // ZQM service
   sc = serviceLocator()->service("ZeroMQSvc", m_zmqSvc, true);
   if( !sc.isSuccess() ) {
      fatal() << "ZeroMQSvc not found" << endmsg;
      return sc;
   }

   if (std::find(begin(m_partitions), end(m_partitions), m_partition) == end(m_partitions)) {
      info() << "Running in partition: " << m_partition
             << ", not starting service." << endmsg;
   }
   return sc;
}

//===============================================================================
void Hlt2MonBaseSvc::handle(const Incident& /* incident */)
{

}

//===============================================================================
StatusCode Hlt2MonBaseSvc::start()
{
   auto sc = Service::start();
   if (!sc.isSuccess()) return sc;

   if (!m_enabled) return sc;

   if (m_thread) {
      sendString(*m_control, "RESUME");
      return sc;
   }

   m_thread = new std::thread([this]{function();});

   m_control = new zmq::socket_t{context(), ZMQ_PUB};
   m_control->bind(m_ctrlCon.c_str());
   return sc;
}


//===============================================================================
StatusCode Hlt2MonBaseSvc::stop()
{
   if (m_control) {
      sendString(*m_control, "PAUSE");
   }
   return StatusCode::SUCCESS;
}

//===============================================================================
StatusCode Hlt2MonBaseSvc::finalize()
{
   // terminate the proxy
   if (m_thread) {
      sendString(*m_control, "TERMINATE");
      m_thread->join();
      delete m_thread;
      m_thread = nullptr;
      int linger = 0;
      m_control->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
      delete m_control;
      m_control = nullptr;
   }

   if (m_incidentSvc) {
      m_incidentSvc->removeListener(this);
      m_incidentSvc->release();
      m_incidentSvc = 0;
   }
   return Service::finalize();
}

//===============================================================================
std::pair<Monitoring::RunNumber, Monitoring::HistId>
Hlt2MonBaseSvc::receiveRunAndId(zmq::socket_t& socket, bool* more) const
{
   // Incoming IDs
   zmq::message_t message;
   socket.recv(&message);
   Monitoring::RunNumber run{0};
   memcpy(&run, message.data(), sizeof(run));

   // Histogram ID
   message.rebuild(sizeof(Monitoring::HistId));
   socket.recv(&message);
   Monitoring::HistId id{0};
   memcpy(&id, message.data(), sizeof(id));

   if (more) *more = message.more();
   return make_pair(run, id);
}

//===============================================================================
std::string Hlt2MonBaseSvc::receiveString(zmq::socket_t& socket, bool* more) const
{
   zmq::message_t msg;
   socket.recv(&msg);
   if (more) *more = msg.more();

   string r(msg.size() + 1, char{});
   r.assign(static_cast<char*>(msg.data()), msg.size());
   return r;
}
