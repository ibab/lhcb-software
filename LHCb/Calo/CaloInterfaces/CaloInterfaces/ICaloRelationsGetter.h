#ifndef ICALORELATIONSGETTER_H 
#define ICALORELATIONSGETTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//Forward dec'
template <class FROM,class TO,class WEIGHT> class IRelationWeighted; 
template <class FROM,class TO,class WEIGHT> class IRelationWeighted2D; 
template <class FROM,class TO> class IRelation; 

namespace LHCb{ 
  class CaloHypo;
  class CaloCluster;
  class Track;
}

static const InterfaceID IID_ICaloRelationsGetter ( "ICaloRelationsGetter", 1, 0 );

/** @class ICaloRelationsGetter ICaloRelationsGetter.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2013-10-04
 */
class ICaloRelationsGetter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloRelationsGetter; }


  virtual IRelationWeighted2D< LHCb::Track , LHCb::CaloHypo , float >* getTrHypoTable2D(std::string location)=0;
  virtual IRelation< LHCb::CaloHypo , float >*  getHypoEvalTable(std::string location)=0;
  virtual IRelationWeighted< LHCb::CaloCluster , LHCb::Track , float >* getClusTrTable  (std::string location)=0;


protected:

private:

};
#endif // ICALORELATIONSGETTER_H
