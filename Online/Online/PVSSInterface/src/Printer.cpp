// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/Printer.cpp,v 1.4 2007-04-26 18:24:24 frankb Exp $
//  ====================================================================
//  Printer.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: Printer.cpp,v 1.4 2007-04-26 18:24:24 frankb Exp $

// Framework include files
#include "PVSS/Printer.h"
#include "PVSS/CfgType.h"
#include "PVSS/CfgDetail.h"
#include "PVSS/CfgAttribute.h"
#include "PVSS/CfgManager.h"

#include "PVSS/DevType.h"
#include "PVSS/DevTypeElement.h"
#include "PVSS/DevTypeManager.h"
#include "PVSS/ControlsManager.h"

#include "PVSS/Internals.h"

#include <iostream>
#include <algorithm>
#include <iomanip>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {


  /** @class Printer   Printer.h  PVSS/Printer.h
    *
    *  Print PVSS items.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class PrinterImp : public Printer {
    /// Reference to output stream
    std::ostream&  m_stream;

  public:
    /// Standard constructor
    PrinterImp(std::ostream& os);
    /// Default destructor
    virtual ~PrinterImp();
    
    /// Configuration named identified printer
    virtual void print(const NamedIdentified& obj);

    /// Configuration attribute printer
    virtual void print(const CfgAttribute& obj);
    /// Configuration detail printer
    virtual void print(const CfgDetail& obj);
    /// Configuration type printer
    virtual void print(const CfgType& obj);

    /// Device type printer
    virtual void print(const DevType& obj);
    /// Device type element printer
    virtual void print(const DevTypeElement& obj);

    /// Device type manager printer
    virtual void print(const DevTypeManager& obj);
    /// Configuration manager printer
    virtual void print(const CfgManager& obj);
    /// Controls manager type printer
    virtual void print(const ControlsManager& obj);

    /// Printout to logger window
    virtual void log(int severity, int type, const std::string& message);
    /// Informational printout to logger window
    virtual void info(int type, const std::string& message);
    /// Warning printout to logger window
    virtual void warning(int type, const std::string& message);
    /// Sever error printout to logger window
    virtual void error(int type, const std::string& message);
    /// Fatal printout to logger window. kill the program instance!
    virtual void fatal(int type, const std::string& message);
  };

  template <typename T> struct ObjectPrint {
    /// Reference to output stream
    std::ostream&      m_stream;
    const std::string& m_ident;
    const std::string& m_prefix;
    ObjectPrint(std::ostream& os, const std::string& ident, const std::string& prefix)
    : m_stream(os), m_ident(ident), m_prefix(prefix) {}
    bool operator()(const T* obj) const {  return true;  }
    bool operator()(const std::pair<std::string,T*>& obj) const {
      return (*this)(obj.second);
    }
  };
  template <> bool ObjectPrint<CfgAttribute>::operator()(const CfgAttribute* obj)  const;
  template <> bool ObjectPrint<CfgDetail>::operator()(const CfgDetail* obj) const;
  template <> bool ObjectPrint<CfgType>::operator()(const CfgType* obj) const;
  template <> bool ObjectPrint<CfgManager>::operator()(const CfgManager* obj) const;
  template <> bool ObjectPrint<ControlsManager>::operator()(const ControlsManager* obj) const;
  template <> bool ObjectPrint<NamedIdentified>::operator()(const NamedIdentified* obj) const;
} 

using namespace PVSS;
#include <iostream>

namespace PVSS {
  template <> bool ObjectPrint<CfgAttribute>::operator()(const CfgAttribute* obj)  const {
    m_stream << m_ident << m_prefix 
	     << "  [id=" << obj->id() << "]" 
	     << obj->name() 
	     << std::endl;
    return true;
  }
  template <> bool ObjectPrint<CfgDetail>::operator()(const CfgDetail* obj) const {
    m_stream << m_ident << m_prefix << obj->name() << "  [id=" << obj->id() << "]" << std::endl;
    return true;
  }
  template <> bool ObjectPrint<CfgType>::operator()(const CfgType* obj) const {
    m_stream << m_ident << m_prefix << obj->name() << "  [id=" << obj->id() << "]" 
	     << " No.Attr:" << obj->attributes().size()
	     << " No.Detail:" << obj->details().size()
	     << std::endl;
    if ( obj->attributes().empty() )  {
      m_stream << "    NO Attributes found" << std::endl;
    }
    else  {
      m_stream << "    " << obj->attributes().size() << " Attributes:" << std::endl;
      std::for_each(obj->attributes().begin(),obj->attributes().end(),ObjectPrint<CfgAttribute>(m_stream,"      ",""));
    }
    if ( obj->details().empty() )  {
      m_stream << "    NO Details found" << std::endl;
    }
    else  {
      m_stream << "    " << obj->details().size() << " Details:" << std::endl;
      std::for_each(obj->details().begin(),obj->details().end(),ObjectPrint<CfgDetail>(m_stream,"      ",""));
    }
    m_stream << std::flush;
    return true;
  }

  template <> bool ObjectPrint<DevType>::operator()(const DevType* obj) const {
    m_stream << m_ident << m_prefix << obj->name() << "  [id=" << obj->id() << "]" 
	     << " No.Elements:" << obj->elements().size() << std::endl;
    if ( obj->elements().empty() )  {
      m_stream << "    NO Elements found" << std::endl;
    }
    else  {
      m_stream << "    " << obj->elements().size() << " Types:" << std::endl;
      std::for_each(obj->elements().begin(),obj->elements().end(),ObjectPrint<DevTypeElement>(m_stream,"      ",""));
    }
    return true;
  }
  template <> bool ObjectPrint<DevTypeElement>::operator()(const DevTypeElement* obj) const {
    m_stream << m_ident << m_prefix 
	     << "ID=" << std::setw(4) << std::left << obj->id() << "   " 
      //<< " DevType:" << std::setw(16) << obj->type()->name() 
	     << " Type[" << std::setw(2) << std::left << obj->content() << "]:" 
	     << std::setw(22) << std::left << obj->typeName()
	     << obj->name() 
	     << std::endl;
    return true;
  }
  template <> bool ObjectPrint<CfgManager>::operator()(const CfgManager* obj) const {
    m_stream << m_ident << m_prefix << obj->name() << "  [id=" << obj->id() << "]" << std::endl;
    return true;
  }
  template <> bool ObjectPrint<DevTypeManager>::operator()(const DevTypeManager* obj) const {
    m_stream << m_ident << m_prefix << obj->name() << "  [id=" << obj->id() << "]" << std::endl;
    if ( obj->types().empty() )  {
      m_stream << "    NO Known device types." << std::endl;
    }
    else {
      m_stream << "    Device Types [" << obj->types().size() << "]:" << std::endl;
      std::for_each(obj->types().begin(),obj->types().end(),
		    ObjectPrint<DevType>(m_stream,"  ","Type:"));
    }
    return true;
  }
  template <> bool ObjectPrint<ControlsManager>::operator()(const ControlsManager* obj) const {
    m_stream << m_ident << m_prefix << obj->name() << "  [id=" << obj->id() << "]" << std::endl;
    return true;
  }
  template <> bool ObjectPrint<NamedIdentified>::operator()(const NamedIdentified* obj) const {
    m_stream << m_ident << m_prefix << obj->name() << "  [id=" << obj->id() << "]" << std::endl;
    return true;
  }
}

std::auto_ptr<Printer> PVSS::createAsciiPrinter(std::ostream& os)    {
  return std::auto_ptr<Printer>(new PrinterImp(os));
}

std::auto_ptr<Printer> PVSS::createLogger()    {
  return std::auto_ptr<Printer>(new PrinterImp(std::cout));
}

/// Initializing constructor
PrinterImp::PrinterImp(std::ostream& os)
: m_stream(os)
{
}

/// Initializing constructor
PrinterImp::~PrinterImp()
{
}

/// Configuration attribute printer
void PrinterImp::print(const CfgAttribute& obj)    {
  ObjectPrint<CfgAttribute>(m_stream,"      ","")(&obj);
}

/// Configuration detail printer
void PrinterImp::print(const CfgDetail& obj)    {
  ObjectPrint<CfgDetail>(m_stream,"      ","")(&obj);
}

/// Configuration named identified printer
void PrinterImp::print(const NamedIdentified& obj)    {
  ObjectPrint<NamedIdentified>(m_stream,"      ","")(&obj);
}

/// Configuration type printer
void PrinterImp::print(const CfgType& obj)    {
  ObjectPrint<CfgType>(m_stream," ","Configuration:")(&obj);
}

/// Device type printer
void PrinterImp::print(const DevType& obj)    {
  ObjectPrint<DevType>(m_stream," ","Type:")(&obj);
}

/// Device type element printer
void PrinterImp::print(const DevTypeElement& obj)    {
  ObjectPrint<DevTypeElement>(m_stream," ","Type:")(&obj);
}

/// Configuration manager printer
void PrinterImp::print(const DevTypeManager& obj)   {
  ObjectPrint<DevTypeManager>(m_stream,"  ","Device Type Manager:")(&obj);
  m_stream << std::flush;
}

/// Configuration manager printer
void PrinterImp::print(const CfgManager& obj)   {
  ObjectPrint<CfgManager>(m_stream,"  ","Configuration Manager:")(&obj);
  if ( !obj.configurations().empty() )  {
    m_stream << "    Configurations [" << obj.configurations().size() << "]:" << std::endl;
    std::for_each(obj.configurations().begin(),obj.configurations().end(),
                  ObjectPrint<CfgType>(m_stream,"  ","Configuration:"));
  }
  m_stream << std::flush;
}

/// Controls manager type printer
void PrinterImp::print(const ControlsManager& obj)    {
  ObjectPrint<ControlsManager>(m_stream,"","Controls Manager:")(&obj);
  //print(*(obj.configMgr()));
  //print(*(obj.typeMgr()));
  m_stream << std::flush;
}

/// Printout to logger window
void PrinterImp::log(int severity, int type, const std::string& message)  {
  pvss_print(severity,type,message.c_str());
}

/// Informational printout to logger window
void PrinterImp::info(int type, const std::string& message)  {
  log(PRIO_INFO,type,message.c_str());
}

/// Warning printout to logger window
void PrinterImp::warning(int type, const std::string& message)  {
  log(PRIO_WARNING,type,message.c_str());
}

/// Sever error printout to logger window
void PrinterImp::error(int type, const std::string& message)  {
  log(PRIO_SEVERE,type,message.c_str());
}

/// Fatal printout to logger window. kill the program instance!
void PrinterImp::fatal(int type, const std::string& message)  {
  log(PRIO_FATAL,type,message.c_str());
}
