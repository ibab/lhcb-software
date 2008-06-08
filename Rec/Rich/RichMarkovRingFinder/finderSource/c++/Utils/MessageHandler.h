// $Id: MessageHandler.h,v 1.2 2008-06-08 12:54:55 jonrob Exp $
#ifndef UTILS_MESSAGEHANDLER_H
#define UTILS_MESSAGEHANDLER_H 1

// Include files
#include <string>

namespace Lester
{

  //--------------------------------------------------------------------------
  /** @enum MessageLevel
   *
   *  Message level
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //--------------------------------------------------------------------------
  enum MessageLevel
    {
      VERBOSE = 1,
      DEBUG,
      INFO,
      WARNING,
      ERROR,
      FATAL
    };

  /** @class IMessageHandler MessageHandler.h Utils/MessageHandler.h
   *
   *  Virtual interface for objects wishing to receive messages
   *
   *  @author Chris Jones
   *  @date   2008-06-08
   */
  class IMessageHandler
  {
  public:
    /// Handle a message of the given level
    virtual void lesterMessage( const Lester::MessageLevel level,
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
    MessageHandler( ) : m_handler(NULL) { }

    /// Destructor
    ~MessageHandler( ) { }

  public:

    /// Declare IMessageHandler to send messages to
    void declare( const Lester::IMessageHandler * handler )
    {
      m_handler = handler;
    }

    /// Send a message
    void message( const Lester::MessageLevel level,
                  const std::string & message ) const;

  private:

    /// message handler
    const Lester::IMessageHandler * m_handler;

  };

  /// Text conversion for Lester::MessageLevel enumeration
  std::string text( const Lester::MessageLevel level );

  /// Access the messaging object
  MessageHandler & messHandle();

}

#endif // UTILS_MESSAGEHANDLER_H
