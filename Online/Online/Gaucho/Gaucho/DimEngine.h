//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/Gaucho/DimEngine.h,v 1.3 2004-09-21 16:35:03 mato Exp $
#ifndef GAUCHO_DIMENGINE_H
#define GAUCHO_DIMENGINE_H 1

#include <string>
#include <map>
#include "dis.hxx"

/** @class DimEngine DimEngine.h Gaucho/DimEngine.h

    This class publishes Gaudi variables as Dim services.

    @author Philippe Vannerem
*/

namespace AIDA {class IHistogram1D;}
class DimH1D;
class IMessageSvc;

class DimEngine {
public:
  DimEngine(char* nodename, ISvcLocator* svclocator);
  virtual ~DimEngine();
  void declSvc(std::string InfoName, int& InfoVar);
  void declSvc(std::string InfoName, float& InfoVar);
  void declSvc(std::string InfoName, char* InfoVar);
  void declSvc(std::string InfoName, AIDA::IHistogram1D* InfoVar);
  void undeclSvc(std::string InfoName);
//! problem in undeclSvc if e.g. float and H1D with same name exist
private:
  char* m_nodename;
  IMessageSvc* m_msgsvc;
  typedef std::map<std::string, DimService*, std::less<std::string> > DimServiceMap;
  typedef DimServiceMap::iterator DimServiceMapIt;
  DimServiceMap m_dimInfos;
  DimServiceMapIt m_dimInfosIt;
  typedef std::map<std::string, DimH1D*, std::less<std::string> > DimH1DMap;
  typedef DimH1DMap::iterator DimH1DMapIt;
  DimH1DMap m_dimHistos;
  DimH1DMapIt m_dimHistosIt;
}; 

#endif // GAUCHO_DIMENGINE_H
