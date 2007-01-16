#ifndef GAUCHO_DIMHISTO_H
#define GAUCHO_DIMHISTO_H 1

#include <string>
#include <iostream>
#include "GaudiKernel/IMessageSvc.h"
#include "dis.hxx"


/** @class DimHistoService Gaucho/DimHistoService.h
    
This class publishes an 1d, 2d or 1d profile histograms as defined in the AIDA interface
as a Dim service. The service updates histo data content from AIDA histo each
time the service is called by a client.

@author Philippe Vannerem
@author Jose Helder Lopes, July 2006
@author Jose Helder Lopes, 27/07/2006
@author Jose Helder Lopes 2006/12/26: Modified to publish AIDA::IProfile1D histos

*/

//forward declaration
namespace AIDA {
  class IHistogram1D;
  class IHistogram2D;
  class IProfile1D;
}

class DimService;

class DimHistoService : public DimService {
public: 
  DimHistoService(std::string hname, const AIDA::IBaseHistogram* InfoVar, int size, ISvcLocator* svclocator);
  virtual ~DimHistoService();
private:
  void serviceHandler();
  std::string m_name;
  int m_dimension;
  int m_size;
  int m_nXBins;
  int m_nYBins;
  float* m_data;
  const AIDA::IHistogram1D* m_h1D;
  const AIDA::IHistogram2D* m_h2D;
  const AIDA::IProfile1D  * m_hprof;
  IMessageSvc* m_msgsvc;
};

// Intermediate class that holds the real DIM service
// Needed because DimHistoService expect the size of the object, which is calculated by this DimHisto.
class DimHisto {
public: 
  DimHisto(std::string hname, const AIDA::IBaseHistogram* InfoVar, ISvcLocator* svclocator);
  virtual ~DimHisto();
private:
  DimHistoService* m_dimHistoScv;
  
  IMessageSvc* m_msgsvc;
};
#endif // GAUCHO_DIMHISTO_H
