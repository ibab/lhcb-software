#ifndef GAUCHO_MONITORSVC_H
#define GAUCHO_MONITORSVC_H 1

#include <string>
#include <map>
#include "GaudiKernel/Service.h"
#include "Gaucho/IMonitorSvc.h"

/** @class MonitorSvc MonitorSvc.h Gaucho/MonitorSvc.h

    This class implements the IMonitorSvc interface, and publishes Gaudi variables
    to outside monitoring processes with Dim.
    
    An internal DimEngine is used for this purpose. A DimPropServer is started which
    takes string commands in the format Algorithm.Property and returns the value of
    the property.


    @author Philippe Vannerem
*/

using AIDA::IHistogram1D;

// Forward declarations
class ISvcLocator;
class DimPropServer;
class DimEngine;


class MonitorSvc : public Service, virtual public IMonitorSvc {
public:
  MonitorSvc(const std::string& name, ISvcLocator* sl);
  virtual ~MonitorSvc() {}
//IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
// Service pure virtual member functions
  StatusCode initialize();
  StatusCode finalize();
  void DeclInfo(std::string InfoName, int& InfoVar);
  void UndeclInfo(std::string infoName, int& InfoVar);
  void PublInfo(std::string InfoName, int& InfoValue);

  void DeclInfo(std::string InfoName, float& InfoVar);
  void UndeclInfo(std::string infoName, float& InfoVar);
  void PublInfo(std::string InfoName, float& InfoValue);

  void DeclInfo(std::string InfoName, char* InfoVar);
  void UndeclInfo(std::string infoName, char* InfoVar);
  void PublInfo(std::string InfoName, char* InfoValue);

  void DeclInfo(std::string InfoName, IHistogram1D* InfoVar);
  void UndeclInfo(std::string infoName, IHistogram1D* InfoVar);
  void PublInfo(std::string InfoName, IHistogram1D* InfoValue);

// automatically called by Dim
//  void DimStart();

private:
  char* m_nodename;
  DimPropServer* m_dimpropsvr;
  DimEngine* m_dimeng;
 };

#endif  //GAUCHO_MONITORSVC_H
