// $Id: selection.h,v 1.5 2006-06-11 20:43:14 ibelyaev Exp $
// ====================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $ 
// ====================================================================
// Incldue files
// ====================================================================
// STD & STL
// ====================================================================
#include <string>
#include <vector>
// ====================================================================
// Relations
// ====================================================================
#include "Relations/RelationsDict.h"
// ====================================================================
// Provided include files: 
// ====================================================================
#include "Event/MCParticle.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
// ====================================================================
// Other include files: put your own includes here:
// ====================================================================
//#include ...
//#include ...
//#include ...
// ====================================================================

/** @file 
  *       
  * Helper file for building Reflex dictionaries for Relations 
   * @author ibelyaev 
  * @date  2006-06-11 22:17:54.560281 
  *       
  */      

namespace Dict
{
  struct __Instantiations 
  {
GaudiDict::Relation1DDict<LHCb::Track,float>	_1 ;
GaudiDict::RelationWeighted1DDict<LHCb::CaloCluster,LHCb::MCParticle,float>	_2 ;
GaudiDict::RelationWeighted1DDict<LHCb::CaloDigit,LHCb::MCParticle,float>	_3 ;
GaudiDict::RelationWeighted1DDict<LHCb::ProtoParticle,LHCb::MCParticle,double>	_4 ;
GaudiDict::RelationWeighted2DDict<LHCb::CaloCluster,LHCb::Track,float>	_5 ;
GaudiDict::RelationWeighted2DDict<LHCb::CaloHypo,LHCb::Track,float>	_6 ;
GaudiDict::RelationWeighted2DDict<LHCb::Track,LHCb::CaloCluster,float>	_7 ;
GaudiDict::RelationWeighted2DDict<LHCb::Track,LHCb::CaloHypo,float>	_8 ;
  };
};

// ====================================================================
// The END ============================================================
// ====================================================================
