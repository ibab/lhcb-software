// $Id: IOMAMsgTool.h,v 1.1 2008-08-19 22:45:31 ggiacomo Exp $
#ifndef OMALIB_IOMAMSGTOOL_H 
#define OMALIB_IOMAMSGTOOL_H 1


#include <string>
#include "GaudiKernel/IAlgTool.h"

class OMAMessage;

static const InterfaceID IID_IOMAMsgTool ( "IOMAMsgTool", 1, 0 );

/** @class IOMAMsgTool IOMAMsgTool.h OMAlib/IOMAMsgTool.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2008-08-14
 */
class IOMAMsgTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IOMAMsgTool; }
  virtual bool raiseAlarm(OMAMessage& message) =0;
  virtual bool lowerAlarm(OMAMessage& message) =0;

private:

};
#endif // OMALIB_IOMAMSGTOOL_H
