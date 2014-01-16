// $Id: MessageLine.h,v 1.2 2008-05-27 06:52:49 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/MessageLine.h,v 1.2 2008-05-27 06:52:49 frankb Exp $
#ifndef ROLOGGER_MESSAGELINE_H
#define ROLOGGER_MESSAGELINE_H

// C++ include files
#include <string>

/*
 *   ROLine namespace declaration
 */
namespace ROLogger {

  // Forward declarations

  /**@class MessageLine MessageLine.h ROLine/MessageLine.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class MessageLine  {
  public:
    enum MsgType { NONE=-1, OTHER=0 };
    enum msg_Level {
      Msg_Verbose = 1,
      Msg_Debug,
      Msg_Info,
      Msg_Warning,
      Msg_Error,
      Msg_Fatal,
      Msg_Always
    };

  protected:
    typedef std::string _S;
    std::string  m_buff;
    mutable int  m_type;
    mutable int  m_utgid;
    mutable int  m_comp;
    mutable int  m_mess;
    enum {
      DATE_START = 0,
      DATE_LEN = 12,
      SEV_START = 13,
      NOD_START = 19,
      LAST
    };
    int analyze() const;
  public:
    /// Standard constructor
    MessageLine() : m_type(NONE)  {  }
    /// Constructor with message information
    MessageLine(const std::string& m) : m_buff(m), m_type(NONE) {}
    /// Copy constructor
    MessageLine(const MessageLine& copy);
    /// Standard destructor
    ~MessageLine() {}
    /// Assignment operator
    MessageLine& operator=(const MessageLine& c)  {
      if ( &c != this )  {
	m_buff = c.m_buff;
	m_type = c.m_type;
	m_utgid = c.m_utgid;
	m_comp = c.m_comp;
	m_mess = c.m_mess;
      }
      return *this;
    }
    std::string date() const      {  return m_buff.substr(DATE_START,DATE_LEN);                             }
    std::string severity() const  {  return m_buff.substr(SEV_START,m_buff.find("]",SEV_START)-SEV_START);  }
    std::string node()  const     {  return m_buff.substr(NOD_START,m_buff.find(":",NOD_START)-NOD_START);  }
    std::string utgid() const     {  return type()>0  ? m_buff.substr(m_utgid,m_comp-m_utgid-3) : _S("");   }
    std::string component() const {  return type()>0  ? m_buff.substr(m_comp,m_mess-m_comp-2) : _S("");     }
    std::string message() const   {  return type()>=0 ? m_buff.substr(m_mess) : _S("");                     }
    int type() const              {  return m_type!=NONE ? m_type : analyze();                              }
    void dump() const;
    /// Access severity level from message content
    int msgSeverity() const;
    /// Access severity level from message content
    static int msgSeverity(const char* msg);
    /// Convert severity string to enum
    static int severityLevel(const std::string& severity);
  };
}
#endif /* ROLOGGER_MESSAGELINE_H */
