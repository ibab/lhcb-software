#ifndef COMPONENT_ZEROMQSVC_H
#define COMPONENT_ZEROMQSVC_H 1

// Include files

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiKernel/IIncidentListener.h>

// ZeroMQ
#include <zmq/zmq.hpp>
#include <ZeroMQ/IZeroMQSvc.h>

/** @class ZeroMQSvc ZeroMQSvc.h
 *
 *
 *  @author Roel Aaij
 *  @date   2015-06-22
 */
class ZeroMQSvc : public extends1<Service, IZeroMQSvc> {
 public:
   /// Standard constructor
   ZeroMQSvc(const std::string& name, ISvcLocator* sl);

   virtual ~ZeroMQSvc( ); ///< Destructor

   // Service pure virtual member functions
   virtual StatusCode initialize();
   virtual StatusCode start();
   virtual StatusCode finalize();

   virtual zmq::context_t& context() const;

   zmq::socket_t socket(int type) const {
      return zmq::socket_t{context(), type};
   }
   
private:

   // properties
   int m_nThreads;

   // data members
   mutable zmq::context_t* m_context;

};
#endif // COMPONENT_ZEROMQSVC_H
