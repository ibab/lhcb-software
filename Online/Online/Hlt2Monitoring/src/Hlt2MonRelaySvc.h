#ifndef HLT2MONRELAYSVC_H
#define HLT2MONRELAYSVC_H

// STD & STL
#include <string>
#include <map>
#include <set>

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiKernel/IIncidentListener.h>

class ISvcLocator;
class IIncidentSvc;

/** @class Hlt2MonRelaySvc

 */

class Hlt2MonRelaySvc : public extends1<Service, IIncidentListener> {
public:

   Hlt2MonRelaySvc(const std::string& name, ISvcLocator* sl);
   virtual ~Hlt2MonRelaySvc();

   // Service pure virtual member functions
   virtual StatusCode initialize();
   virtual StatusCode start();
   virtual StatusCode stop();
   virtual StatusCode finalize();

   /// Incident handler implemenentation: Inform that a new incident has occured
   virtual void handle(const Incident& inc);

private:

   // properties
   std::string m_hostRegex;
   bool m_top;
   bool m_forceTop;
   bool m_relay;
   std::string m_partition;
   std::vector<std::string> m_partitions;
   std::string m_frontCon;
   std::string m_backCon;

   unsigned int m_inPort;
   unsigned int m_outPort;
   IIncidentSvc* m_incidentSvc;
   std::thread* m_thread;
   zmq::socket_t* m_control;
   zmq::context_t* m_context;

};

#endif // HLT2MONRELAYSVC_H
