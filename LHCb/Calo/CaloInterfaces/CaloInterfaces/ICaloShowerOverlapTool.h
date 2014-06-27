#ifndef ICALOSHOWEROVERLAPTOOL_H 
#define ICALOSHOWEROVERLAPTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb{
  class CaloCluster;
}

static const InterfaceID IID_ICaloShowerOverlapTool ( "ICaloShowerOverlapTool", 1, 0 );

/** @class ICaloShowerOverlapTool ICaloShowerOverlapTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2014-06-03
 */
class ICaloShowerOverlapTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloShowerOverlapTool; }

  virtual StatusCode initialize()=0;
  virtual StatusCode setProfile(std::string)=0;
  virtual void process(const LHCb::CaloCluster* c1,const LHCb::CaloCluster* c2,
                       int spd=0,int niter=5,bool propagateInitialWeights=false)=0;
  

protected:

private:

};
#endif // ICALOSHOWEROVERLAPTOOL_H
