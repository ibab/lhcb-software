// $Id: selPhys.h,v 1.1 2009-11-05 07:30:55 jpalac Exp $
// ====================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
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
#include "Event/Particle.h"
#include "Event/Track.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/VertexBase.h"
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
   * @author jpalac 
  * @date  2009-11-05 08:29:41.914054 
  *       
  */      

namespace Dict
{
  struct __Instantiations 
  {
        GaudiDict::Relation1DDict<LHCb::Particle,LHCb::VertexBase>	_1 ;
        GaudiDict::Relation1DDict<LHCb::Track,float>	_2 ;
        GaudiDict::RelationWeighted1DDict<LHCb::Particle,LHCb::Particle,double>	_3 ;
        GaudiDict::RelationWeighted1DDict<LHCb::Particle,LHCb::VertexBase,double>	_4 ;
        GaudiDict::RelationWeighted2DDict<LHCb::CaloCluster,LHCb::Track,float>	_5 ;
        GaudiDict::RelationWeighted2DDict<LHCb::CaloHypo,LHCb::Track,float>	_6 ;
        GaudiDict::RelationWeighted2DDict<LHCb::Track,LHCb::CaloCluster,float>	_7 ;
        GaudiDict::RelationWeighted2DDict<LHCb::Track,LHCb::CaloHypo,float>	_8 ;
  };
} // end of namespace Dict 

// ====================================================================
// The END ============================================================
// ====================================================================
