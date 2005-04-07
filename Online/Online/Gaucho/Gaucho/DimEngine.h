//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/Gaucho/DimEngine.h,v 1.5 2005-04-07 14:42:00 evh Exp $
#ifndef GAUCHO_DIMENGINE_H
#define GAUCHO_DIMENGINE_H 1

#include <string>
#include <map>
#include "dis.hxx"


/** @class DimEngine DimEngine.h Gaucho/DimEngine.h
    
This class publishes Gaudi variables as Dim services.

@author Philippe Vannerem
*/

namespace AIDA {class IHistogram;
class IHistogram1D;
class IHistogram2D;
}
class DimH1D;
class DimH2D;
class IMessageSvc;

class DimEngine {
public:
  DimEngine(char* nodename, ISvcLocator* svclocator);
  virtual ~DimEngine();
  void declSvc(std::string InfoName, const bool& InfoVar);
  void declSvc(std::string InfoName, const int& InfoVar);
  void declSvc(std::string InfoName, const long& InfoVar);
  void declSvc(std::string InfoName, const float& InfoVar);
  void declSvc(std::string InfoName, const double& InfoVar);
  void declSvc(std::string InfoName, const char* InfoVar);
  void declSvc(std::string InfoName, const std::string& InfoVar);
//  void declSvc(std::string InfoName, const std::pair<double,double>& InfoVar);
  void declSvc(std::string InfoName, const AIDA::IHistogram* InfoVar);
  void undeclSvc(std::string InfoName);
private:
  std::string m_nodename;
  IMessageSvc* m_msgsvc;
  typedef std::map<std::string, DimService*, std::less<std::string> > DimServiceMap;
  typedef DimServiceMap::iterator DimServiceMapIt;
  DimServiceMap m_dimInfos;
  DimServiceMapIt m_dimInfosIt;
  typedef std::map<std::string, DimH1D*, std::less<std::string> > DimH1DMap;
  typedef DimH1DMap::iterator DimH1DMapIt;
  DimH1DMap m_dim1DHistos;
  DimH1DMapIt m_dim1DHistosIt;

  typedef std::map<std::string, DimH2D*, std::less<std::string> > DimH2DMap;
  typedef DimH2DMap::iterator DimH2DMapIt;
  DimH2DMap m_dim2DHistos;
  DimH2DMapIt m_dim2DHistosIt;

}; 

#endif // GAUCHO_DIMENGINE_H
