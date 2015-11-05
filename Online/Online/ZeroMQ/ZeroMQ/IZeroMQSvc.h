#ifndef ZEROMQ_IZEROMQSVC_H
#define ZEROMQ_IZEROMQSVC_H 1

// Include files
// from STL
#include <type_traits>
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/StringKey.h"

// ZeroMQ
#include <zmq/zmq.hpp>

namespace {
   template <class T>
   size_t defaultSizeOf(const T&) {
      return sizeof(T);
   }
}

/** @class IZeroMQSvc IZeroMQSvc.h ZeroMQ/IZeroMQSvc.h
 *
 *
 *  @author
 *  @date   2015-06-22
 */
class GAUDI_API IZeroMQSvc : virtual public INamedInterface {
public:

   // Return the interface ID
   DeclareInterfaceID(IZeroMQSvc, 2, 0);

   virtual ~IZeroMQSvc() {};

   virtual zmq::context_t& context() const = 0;
   virtual zmq::socket_t socket(int type) const = 0;
   
   // receive message with ZMQ, general version
   template <class T>
      typename std::enable_if<!(std::is_same<T, std::string>::value
                                || std::is_same<T, const char*>::value
                                || std::is_same<T, char*>::value), T>::type
      receive(zmq::socket_t& socket, bool* more = nullptr) const {
      zmq::message_t msg;
      socket.recv(&msg);
      if (more) *more = msg.more();
      
      T object;
      memcpy(&object, msg.data(), msg.size());
      return std::move(object);
   }

   // receive message with ZMQ, string version
   template <class T>
      typename std::enable_if<std::is_same<T, std::string>::value, T>::type
      receive(zmq::socket_t& socket, bool* more = nullptr) const {
      zmq::message_t msg;
      socket.recv(&msg);
      if (more) *more = msg.more();
      
      std::string r(msg.size() + 1, char{});
      r.assign(static_cast<char*>(msg.data()), msg.size());
      return r;
   }
      
   // Send message with ZMQ
   template <class T>
   bool send(zmq::socket_t& socket, const T& item, int flag = 0,
             std::function<size_t(const T& t)> sizeFun = defaultSizeOf<T>) const {
      size_t s = sizeFun(item);
      zmq::message_t msg(s);
      memcpy(static_cast<void*>(msg.data()), &item, s);
      return socket.send(msg, flag);
   }

   // Special case for std::string
   bool send(zmq::socket_t& socket, const std::string& item, int flags = 0) const {
      return send<const char>(socket, *item.c_str(), flags, [](const char& cs){ return strlen(&cs); });
   }

   // Special case for const char*
   bool send(zmq::socket_t& socket, const char* item, int flags = 0) const {
      return send<const char>(socket, *item, flags, [](const char& cs){ return strlen(&cs); });
   }
   
};

#endif // ZEROMQ_IZEROMQSVC_H
