// $Id: MessageHandler.h,v 1.3 2008-06-08 15:31:40 jonrob Exp $
#ifndef UTILS_MESSAGEHANDLER_H
#define UTILS_MESSAGEHANDLER_H 1

// Include files
#include <string>
#include <sstream>

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

    inline std::ostringstream& stream() { return m_stream; }

    MessageHandler& operator<<(MessageHandler& (*_f)(MessageHandler&))
    {
      _f(*this);
      return *this;
    }
    MessageHandler& operator<<(std::ostream& (*_f)(std::ostream&))
    {
      _f(m_stream);
      return *this;
    }
    MessageHandler& operator<<(std::ios& (*_f)(std::ios&))
    {
      _f(m_stream);
      return *this;
    }
    MessageHandler& operator<<(std::ios_base& (*_f)(std::ios_base&))
    {
      _f(m_stream);
      return *this;
    }

    MessageHandler& doOutput()
    {
      message( m_level, m_stream.str() );
      m_stream.str("");
      return *this;
    }

  public:

    /// Declare IMessageHandler to send messages to
    inline void declare( const Lester::IMessageHandler * handler )
    { m_handler = handler; }

    /// Send a message
    void message( const Lester::MessageLevel level,
                  const std::string & message ) const;

    /// Send a verbose level message
    inline void verbose( const std::string & mess ) const
    { message( Lester::VERBOSE, mess ); }

    /// Send a debug level message
    inline void debug( const std::string & mess ) const
    { message( Lester::DEBUG, mess ); }

    /// Send an info level message
    inline void info( const std::string & mess ) const
    { message( Lester::INFO, mess ); }

    /// Send a warning level message
    inline void warning( const std::string & mess ) const
    { message( Lester::WARNING, mess ); }

    /// Send an error level message
    inline void error( const std::string & mess ) const
    { message( Lester::ERROR, mess ); }

    /// Send a fatal level message
    inline void fatal( const std::string & mess ) const
    { message( Lester::FATAL, mess ); }

    /// Send an info level message
    inline MessageHandler& verbose()
    { m_level = Lester::VERBOSE; return *this; }

    /// Send an info level message
    inline MessageHandler& debug()
    { m_level = Lester::DEBUG; return *this; }

    /// Send an info level message
    inline MessageHandler& info()
    { m_level = Lester::INFO; return *this; }

    /// Send an info level message
    inline MessageHandler& warning()
    { m_level = Lester::WARNING; return *this; }

    /// Send an info level message
    inline MessageHandler& error()
    { m_level = Lester::ERROR; return *this; }

    /// Send an info level message
    inline MessageHandler& fatal()
    { m_level = Lester::FATAL; return *this; }

  private:

    /// message handler
    const Lester::IMessageHandler * m_handler;

    /// Level
    Lester::MessageLevel m_level;

    /// stream
    std::ostringstream m_stream;

  };

  /// Text conversion for Lester::MessageLevel enumeration
  std::string text( const Lester::MessageLevel level );

  /// Access the messaging object
  MessageHandler & messHandle();

  inline Lester::MessageHandler& endmsg(Lester::MessageHandler& s)
  {
    return s.doOutput();
  }

  template <typename T>
  inline Lester::MessageHandler& operator<< ( Lester::MessageHandler& lhs,
                                              const T& arg )
  {
    lhs.stream() << arg;
    return lhs;
  }

}

#endif // UTILS_MESSAGEHANDLER_H
