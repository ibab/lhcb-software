//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/src/DimEngine.h,v 1.2 2006-07-27 15:45:59 evh Exp $
#ifndef GAUCHO_DIMENGINE_H
#define GAUCHO_DIMENGINE_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>
#include <map>
#include "dis.hxx"


/** @class DimEngine DimEngine.h Gaucho/DimEngine.h
    
This class publishes Gaudi variables as Dim services.

@author Philippe Vannerem
@author Jose Helder Lopes Jan. 2005
@author Jose Helder Lopes 27/07/2006
*/

namespace AIDA {class IHistogram;
  class IHistogram1D;
  class IHistogram2D;
}
class DimHisto;
class IMessageSvc;

class DimEngine : public GaudiAlgorithm {
public:
  DimEngine(std::string name, ISvcLocator* svclocator);
  virtual ~DimEngine();
  void declSvc(std::string InfoName, const bool& InfoVar);
  void declSvc(std::string InfoName, const int& InfoVar);
  void declSvc(std::string InfoName, const long& InfoVar);
  void declSvc(std::string InfoName, const float& InfoVar);
  void declSvc(std::string InfoName, const double& InfoVar);
  void declSvc(std::string InfoName, const char* InfoVar);
  void declSvc(std::string InfoName, const std::string& InfoVar);
  void declSvc(std::string InfoName, const std::pair<double,double>& InfoVar);
  void declSvc(std::string InfoName, const AIDA::IHistogram* InfoVar);
  void undeclSvc(std::string InfoName);
private:
  std::string m_dimName;
  ISvcLocator* m_svclocator;
  IMessageSvc* m_msgsvc;
  // Map relating each info to the DimService address
  typedef std::map<std::string, DimService*, 
                   std::less<std::string> > DimServiceMap;
  typedef DimServiceMap::iterator DimServiceMapIt;
  DimServiceMap m_dimInfos;
  DimServiceMapIt m_dimInfosIt;
  // Map relating each Histo info to the DimHisto address
  typedef std::map<std::string, DimHisto*, std::less<std::string> > DimHistoMap;
  typedef DimHistoMap::iterator DimHistoMapIt;
  DimHistoMap m_dimHistos;
  DimHistoMapIt m_dimHistosIt;
  // Map relating each pair info to the pair of DimService addresses
  typedef std::map<std::string, std::pair<DimService*,DimService*>, 
                   std::less<std::string> > DimPairMap;
  typedef DimPairMap::iterator DimPairMapIt;
  DimPairMap m_dimPairs;
  DimPairMapIt m_dimPairsIt;

}; 

#endif // GAUCHO_DIMENGINE_H
