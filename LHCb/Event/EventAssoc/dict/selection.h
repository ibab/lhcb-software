// $Id: selection.h,v 1.2 2006-02-07 14:41:50 ibelyaev Exp $
// ====================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ 
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
//#include "Event/Track.h"
#include "Event/ProtoParticle.h"
//#include "Event/Particle.h"
//#include "Event/Vertex.h"
#include "Event/MCParticle.h"
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
  * @date  2006-02-07 15:37:05.270000 
  *       
  */      

namespace Dict
{
  struct __Instantiations 
  {
    //GaudiDict::Relation2DDict<LHCb::Particle,LHCb::Vertex>	_1 ;
GaudiDict::RelationWeighted1DDict<LHCb::ProtoParticle,LHCb::MCParticle,double>	_2 ;
    //GaudiDict::Relation1DDict<LHCb::Particle,LHCb::MCParticle>	_3 ;
    //GaudiDict::RelationWeighted2DDict<LHCb::Particle,LHCb::MCParticle,double>	_4 ;
    //GaudiDict::Relation1DDict<LHCb::Track,float>	_5 ;
GaudiDict::RelationWeighted1DDict<LHCb::CaloDigit,LHCb::MCParticle,float>	_6 ;
GaudiDict::RelationWeighted1DDict<LHCb::CaloCluster,LHCb::MCParticle,float>	_7 ;
    //GaudiDict::RelationWeighted2DDict<LHCb::CaloHypo,LHCb::Track,float>	_8 ;
    //GaudiDict::RelationWeighted2DDict<LHCb::CaloCluster,LHCb::Track,float>	_9 ;
  };
};

// ====================================================================
// The END ============================================================
// ====================================================================
