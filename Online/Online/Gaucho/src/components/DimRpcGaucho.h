#ifndef GAUCHO_DIMRPCGAUCHO_H
#define GAUCHO_DIMRPCGAUCHO_H 1

#include "dis.hxx"
#include "GaudiKernel/IHistogramSvc.h"


/** @class DimRpcGaucho DimRpcGaucho.h 
Class to receive rcp string commands from clients 
and apply to Algorithms/Histograms/ApplicationMgr 
@author jotalo
*/

class ISvcLocator;
class IAlgManager;
class IProperty;
class IMessageSvc;
class DataObject;
class IDataManagerSvc;
class IRegistry;

class DimRpcGaucho : public DimRpc {
public:
  DimRpcGaucho(std::string name, ISvcLocator* svcLocator);
  virtual ~DimRpcGaucho();
  void listHistos();
  void resetHistos();
  void rpcHandler();
  std::vector<std::string> histoIds();
  void manageHistos (bool list, bool reset, bool save);
  void histogramIdentifier(IRegistry* object, std::vector<std::string> &idList, bool reset, bool save, int &level);
  void setUtgId(std::string utgId){m_utgId = utgId;}
  IRegistry* rootObject();
private:
  IAlgManager* m_algManager;
  ISvcLocator* m_svcLocator;
  IProperty*   m_applicationMgr;
  IMessageSvc* m_msgSvc;
  IHistogram* m_histogram;
  DataObject* m_dataObject;
  IHistogramSvc* m_histogramSvc;
  std::string m_utgId;
  
};

#endif //GAUCHO_DIMRPCGAUCHO_H
