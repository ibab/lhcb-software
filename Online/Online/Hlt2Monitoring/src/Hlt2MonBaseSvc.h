#ifndef HLT2MONBASESVC_H
#define HLT2MONBASESVC_H

// STD & STL
#include <functional>
#include <string>
#include <thread>
#include <map>
#include <set>

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiKernel/IIncidentListener.h>

// HLT2 monitoring
#include <Hlt2Monitoring/Common.h>

// ZeroMQ
#include <ZeroMQ/IZeroMQSvc.h>

class ISvcLocator;
class IIncidentSvc;

/** @class Hlt2Saver Hlt2Saver.h
 *
 *
 *  @author Roel Aaij
 *  @date   2015-07-06
 */
class Hlt2MonBaseSvc : public extends1<Service, IIncidentListener> {
public:

   Hlt2MonBaseSvc(const std::string& name, ISvcLocator* sl, bool bindControl = false);
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

   IZeroMQSvc& zmq() const { return *m_zmqSvc; }
   inline zmq::socket_t& control() { return *m_control; }
   const std::string& ctrlCon() const { return m_ctrlCon; }

   // Convenience
   virtual zmq::socket_t socket(int type) const {
      return zmq().socket(type);
   }

   template<class... Args>
   bool send(zmq::socket_t& s, Args&&... args) const {
      return zmq().send(s, std::forward<Args>(args)...);
   }

   template<class T, class... Args>
   T receive(zmq::socket_t& s, Args&&... args) const {
      return zmq().receive<T>(s, std::forward<Args>(args)...);
   }
   
   std::pair<Monitoring::RunNumber, Monitoring::HistId> receiveRunAndId(zmq::socket_t& socket, bool* more = nullptr) const;

   // Are we enabled?
   bool enabled() const { return m_enabled; }
   void disable() { m_enabled = false; }
   void enable()  { m_enabled = true; }

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

private:

   // properties
   bool m_bindControl;

   // data members
   bool m_enabled;
   IIncidentSvc* m_incidentSvc;
   std::thread* m_thread;
   zmq::socket_t* m_control;
   std::string m_ctrlCon;
   IZeroMQSvc* m_zmqSvc;

};

#endif // HLT2MONBASESVC_H
