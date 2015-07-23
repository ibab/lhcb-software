#ifndef HLT2MONBASESVC_H
#define HLT2MONBASESVC_H

// STD & STL
#include <string>
#include <functional>
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

namespace {
   template <class T>
   size_t defaultSizeOf(const T&) {
      return sizeof(T);
   }
}

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

   IZeroMQSvc* zmqSvc() const { return m_zmqSvc; }
   inline zmq::context_t& context() const { return m_zmqSvc->context(); }
   inline zmq::socket_t& control() const { return *m_control; }
   const std::string& ctrlCon() const { return m_ctrlCon; }

   // General method to send simple objects
   template <class T>
   bool sendMessage(zmq::socket_t& socket, const T& item, int flag = 0,
                    std::function<size_t(const T& t)> sizeFun = defaultSizeOf<T>) const {
      size_t s = sizeFun(item);
      zmq::message_t msg(s);
      memcpy(static_cast<void*>(msg.data()), &item, s);
      return socket.send(msg, flag);
   }

   bool sendString(zmq::socket_t& socket, const std::string& item, int flags = 0) const {
      return sendMessage<const char>(socket, *item.c_str(), flags, [](const char& cs){ return strlen(&cs); });
   }

   std::string receiveString(zmq::socket_t& socket, bool* more = nullptr) const;
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

   // data members
   bool m_bindControl;
   bool m_enabled;
   IIncidentSvc* m_incidentSvc;
   std::thread* m_thread;
   zmq::socket_t* m_control;
   std::string m_ctrlCon;
   IZeroMQSvc* m_zmqSvc;

};

#endif // HLT2MONBASESVC_H
