// $Id: IErrorLogger.h,v 1.2 2006-04-24 14:45:05 frankb Exp $
#ifndef GAUDIONLINE_IERRORLOGGER_H
#define GAUDIONLINE_IERRORLOGGER_H

// Framework include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"

// C/C++ include files
#include <string>

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IErrorLogger("IErrorLogger", 2 , 0); 

/** @class IDataProviderSvc IDataProviderSvc.h GaudiKernel/IDataProviderSvc.h
 *
 *  Interface to the online line error loggers
 *
 *  @author Markus Frank
 *  @version 1.0
 */
class IErrorLogger  : virtual public IInterface	    {
  public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IErrorLogger; }

  /// Report Error to logger
  /** @param level      [IN]    Message severity level
    * @param src        [IN]    Message source
    * @param msg        [IN]    Message string
    */
  virtual void report(int level, const std::string& src, const std::string& msg) = 0;
};
#endif  // GAUDIONLINE_IERRORLOGGER_H
