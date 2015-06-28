#ifndef HLT2MONBASESVC_H
#define HLT2MONBASESVC_H

// STD & STL
#include <string>
#include <map>
#include <set>

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiKernel/IIncidentListener.h>

// ZeroMQ
#include <ZeroMQ/IZeroMQSvc.h>

class ISvcLocator;
class IIncidentSvc;

/** @class Hlt2MonBaseSvc

 */

class Hlt2MonBaseSvc : public extends1<Service, IIncidentListener> {
public:

   Hlt2MonBaseSvc(const std::string& name, ISvcLocator* sl);
   virtual ~Hlt2MonBaseSvc();

   // Service pure virtual member functions
   virtual StatusCode initialize();
   virtual StatusCode start();
   virtual StatusCode stop();
   virtual StatusCode finalize();

   /// Incident handler implemenentation: Inform that a new incident has occured
   virtual void handle(const Incident& inc);

   virtual void function() = 0;

protected:

   IZeroMQSvc* zmqSvc() const { return m_zmqSvc; }
   inline zmq::context_t& context() const { return m_zmqSvc->context(); }
   inline zmq::socket_t& control() const { return *m_control; }
   const std::string& ctrlCon() const { return m_ctrlCon; }

   // properties
   bool m_top;
   bool m_forceTop;
   std::string m_partition;
   std::vector<std::string> m_partitions;
   std::string m_frontCon;
   std::string m_backCon;
   std::string m_controlCon;
   unsigned int m_inPort;
   unsigned int m_outPort;

   // data members
   bool m_enabled;

private:

   // data members
   IIncidentSvc* m_incidentSvc;
   std::thread* m_thread;
   zmq::socket_t* m_control;
   std::string m_ctrlCon;
   IZeroMQSvc* m_zmqSvc;

};

#endif // HLT2MONBASESVC_H
