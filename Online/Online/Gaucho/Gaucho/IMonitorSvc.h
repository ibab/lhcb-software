#ifndef GAUCHO_IMONITORSVC_H
#define GAUCHO_IMONITORSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>

// Declaration of the interface ID ( interface id, major version, minor version) 
//! still have to change numbers
static const InterfaceID IID_IMonitorSvc(741, 1 , 0);

// forward declaration
namespace AIDA {class IHistogram1D;}

/** @class IMonitorSvc IMonitorSvc.h Gaucho/IMonitorSvc.h

    Definition of the IMonitorSvc interface, which publishes Gaudi variables
    to outside monitoring processes.

    @author Philippe Vannerem
*/

class IMonitorSvc : virtual public IInterface {
  public:
/// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMonitorSvc; }
  // counters
  virtual void DeclInfo(std::string InfoName, int& InfoVar) = 0;
  virtual void UndeclInfo(std::string InfoName, int& InfoVar) = 0;
  virtual void PublInfo(std::string InfoName, int& InfoValue) = 0;

  virtual void DeclInfo(std::string InfoName, float& InfoVar) = 0;
  virtual void UndeclInfo(std::string InfoName, float& InfoVar) = 0;
  virtual void PublInfo(std::string InfoName, float& InfoValue) = 0;

  virtual void DeclInfo(std::string InfoName, char* InfoVar) = 0;
  virtual void UndeclInfo(std::string InfoName, char* InfoVar) = 0;
  virtual void PublInfo(std::string InfoName, char* InfoValue) = 0;

  virtual void DeclInfo(std::string InfoName, AIDA::IHistogram1D* InfoVar) = 0;
  virtual void UndeclInfo(std::string InfoName, AIDA::IHistogram1D* InfoVar) = 0;
  virtual void PublInfo(std::string InfoName, AIDA::IHistogram1D* InfoValue) = 0;

};

#endif // GAUCHO_IMONITORSVC_H
