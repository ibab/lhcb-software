// $Id: IEvtTypeSvc.h,v 1.1 2005-12-02 16:28:07 cattanem Exp $
#ifndef KERNEL_IEVTTYPESVC_H 
#define KERNEL_IEVTTYPESVC_H 1

// Include files
#include "GaudiKernel/IService.h"

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IEvtTypeSvc("IEvtTypeSvc", 1 , 0); 

/** @class IEvtTypeSvc IEvtTypeSvc.h Kernel/IEvtTypeSvc.h
 *
 *  Interface to service that provide information for EVTTYPE code.
 *
 *  @author Gloria CORTI
 *  @date   2004-04-30
 */
class IEvtTypeSvc : virtual public IService {
public: 
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IEvtTypeSvc; }

  /// Given an event code number provide its Nick Name as in input table.
  /// If an event type does not exist in the table this method 
  /// will return "Unknown" 
  virtual std::string nickName( const int evtCode ) = 0;
  
  /// Given an event code number provide the ASCII decay descriptor
  /// as in input table. Some event types have "No signal" as
  /// decay descriptor, if an event type does not exist in the
  /// table this method will return "Unknown" 
  virtual std::string decayDescriptor( const int evtCode ) = 0;

  /// Given an event code number check if it 'exist', i.e. is known by the
  /// service
  virtual bool typeExists( const int evtCode ) = 0;

};
#endif // KERNEL_IEVTTYPESVC_H
