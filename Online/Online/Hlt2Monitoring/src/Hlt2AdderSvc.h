#ifndef HLT2ADDER_H
#define HLT2ADDER_H 1

// Include files
#include <string>
#include <unordered_map>

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiKernel/IIncidentListener.h>

/** @class Hlt2Adder Hlt2Adder.h
 *
 *
 *  @author
 *  @date   2015-06-13
 */
class Hlt2AdderSvc :public extends1<Service, IIncidentListener> {
public:
   /// Standard constructor
   Hlt2AdderSvc(const std::string& name, ISvcLocator* sl);

   virtual ~Hlt2AdderSvc( ); ///< Destructor

   // Service pure virtual member functions
   virtual StatusCode initialize();
   virtual StatusCode start();
   virtual StatusCode stop();
   virtual StatusCode finalize();

   /// Incident handler implemenentation: Inform that a new incident has occured
   virtual void handle(const Incident& inc);

private:

   // properties
   std::string m_partition;
   std::vector<std::string> m_partitions;
   std::string m_frontCon;
   std::string m_backCon;
   unsigned int m_outPort;

   // data members
   bool m_add;
   std::unordered_map<Monitoring::HistId, Monitoring::Histogram> m_histograms;

   IIncidentSvc* m_incidentSvc;
   std::thread* m_thread;
   zmq::socket_t* m_control;
   zmq::context_t* m_context;

};
#endif // HLT2ADDER_H
