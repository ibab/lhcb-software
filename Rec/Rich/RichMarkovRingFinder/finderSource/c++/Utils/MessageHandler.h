// $Id: MessageHandler.h,v 1.1 2008-06-08 11:38:33 jonrob Exp $
#ifndef UTILS_MESSAGEHANDLER_H
#define UTILS_MESSAGEHANDLER_H 1

// Include files
#include <string>

namespace Lester
{

  enum MessageLevel
    {
      VERBOSE = 1,
      DEBUG,
      INFO,
      WARNING,
      ERROR,
      FATAL
    };

  class IMessageHandler
  {
  public:
    /// Handle a message of the given level
    virtual void handleMessage( const Lester::MessageLevel level,
                                const std::string & message ) const = 0;
  };

  /** @class MessageHandler MessageHandler.h Utils/MessageHandler.h
   *
   *  Messaging system for RichMarkovRingFinder
   *
   *  @author Chris Jones
   *  @date   2008-06-08
   */

  class MessageHandler
  {

  public:

    /// Standard constructor
    MessageHandler( ) { }

    ~MessageHandler( ) { }

  protected:

  private:

  };

  /// Access the messaging object
  MessageHandler & messHandle();

}

#endif // UTILS_MESSAGEHANDLER_H
