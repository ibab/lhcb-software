#ifndef ZEROMQ_IZEROMQSVC_H
#define ZEROMQ_IZEROMQSVC_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/StringKey.h"

// ZeroMQ
#include <zmq/zmq.hpp>

/** @class IZeroMQSvc IZeroMQSvc.h ZeroMQ/IZeroMQSvc.h
 *
 *
 *  @author
 *  @date   2015-06-22
 */
class GAUDI_API IZeroMQSvc : virtual public INamedInterface {
public:

   // Return the interface ID
   DeclareInterfaceID(IZeroMQSvc, 1, 0);

   virtual ~IZeroMQSvc() {};

   virtual zmq::context_t& context() const = 0;

};


// Include files
// from STL
#include <string>
#include <vector>


#endif // ZEROMQ_IZEROMQSVC_H
