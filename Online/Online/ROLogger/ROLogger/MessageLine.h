// $Id: MessageLine.h,v 1.1 2008-05-13 07:56:02 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/MessageLine.h,v 1.1 2008-05-13 07:56:02 frankb Exp $
#ifndef ROLOGGER_MESSAGELOGGER_H
#define ROLOGGER_MESSAGELOGGER_H

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
    std::string date() const      {  return m_buff.substr(DATE_START,DATE_LEN);                             }
    std::string severity() const  {  return m_buff.substr(SEV_START,m_buff.find("]",SEV_START)-SEV_START);  }
    std::string node()  const     {  return m_buff.substr(NOD_START,m_buff.find(":",NOD_START)-NOD_START);  }
    std::string utgid() const     {  return type()>0  ? m_buff.substr(m_utgid,m_comp-m_utgid-3) : _S("");   }
    std::string component() const {  return type()>0  ? m_buff.substr(m_comp,m_mess-m_comp-2) : _S("");     }
    std::string message() const   {  return type()>=0 ? m_buff.substr(m_mess) : _S("");                     }
    int type() const              {  return m_type!=NONE ? m_type : analyze();                              }
    void dump() const;
  };
}
#endif /* ROLOGGER_MESSAGELOGGER_H */
