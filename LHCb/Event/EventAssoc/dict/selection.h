// $Id: selection.h,v 1.19 2009-11-04 13:51:26 cocov Exp $
// ====================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.19 $ 
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
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/GenCollision.h"
#include "Event/RecVertex.h"
#include "HepMC/GenParticle.h"
#include "Event/L0CaloCandidate.h"
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
   * @author cocov 
  * @date  2009-11-04 14:04:56.425728 
  *       
  */      

namespace Dict
{
  struct __Instantiations 
  {
        GaudiDict::Relation1DDict<LHCb::MCVertex,LHCb::GenCollision>	_1 ;
        GaudiDict::Relation1DDict<LHCb::Particle,LHCb::MCParticle>	_2 ;
        GaudiDict::Relation1DDict<LHCb::Particle,LHCb::VertexBase>	_3 ;
        GaudiDict::Relation1DDict<LHCb::Track,float>	_4 ;
        GaudiDict::Relation2DDict<HepMC::GenParticle*,LHCb::MCParticle>	_5 ;
        GaudiDict::Relation2DDict<HepMC::GenParticle*,LHCb::Particle>	_6 ;
        GaudiDict::Relation2DDict<LHCb::MCParticle,HepMC::GenParticle*>	_7 ;
        GaudiDict::Relation2DDict<LHCb::MCParticle,LHCb::Particle>	_8 ;
        GaudiDict::Relation2DDict<LHCb::Particle,HepMC::GenParticle*>	_9 ;
        GaudiDict::Relation2DDict<LHCb::Particle,LHCb::MCParticle>	_10 ;
        GaudiDict::RelationWeighted1DDict<LHCb::CaloCluster,LHCb::MCParticle,float>	_11 ;
        GaudiDict::RelationWeighted1DDict<LHCb::CaloDigit,LHCb::MCParticle,float>	_12 ;
        GaudiDict::RelationWeighted1DDict<LHCb::MCParticle,LHCb::CaloCluster,float>	_13 ;
        GaudiDict::RelationWeighted1DDict<LHCb::Particle,LHCb::MCParticle,double>	_14 ;
        GaudiDict::RelationWeighted1DDict<LHCb::Particle,LHCb::Particle,double>	_15 ;
        GaudiDict::RelationWeighted1DDict<LHCb::Particle,LHCb::VertexBase,double>	_16 ;
        GaudiDict::RelationWeighted1DDict<LHCb::ProtoParticle,LHCb::MCParticle,double>	_17 ;
        GaudiDict::RelationWeighted2DDict<LHCb::CaloCluster,LHCb::Track,float>	_18 ;
        GaudiDict::RelationWeighted2DDict<LHCb::CaloHypo,LHCb::Track,float>	_19 ;
        GaudiDict::RelationWeighted2DDict<LHCb::GenCollision,LHCb::RecVertex,std::pair<unsigned int,float> >	_20 ;
        GaudiDict::RelationWeighted2DDict<LHCb::L0CaloCandidate,LHCb::MCParticle,double>	_21 ;
        GaudiDict::RelationWeighted2DDict<LHCb::MCParticle,LHCb::L0CaloCandidate,double>	_22 ;
        GaudiDict::RelationWeighted2DDict<LHCb::MCParticle,LHCb::Particle,double>	_23 ;
        GaudiDict::RelationWeighted2DDict<LHCb::MCParticle,LHCb::Track,double>	_24 ;
        GaudiDict::RelationWeighted2DDict<LHCb::MCVertex,LHCb::RecVertex,std::pair<unsigned int,float> >	_25 ;
        GaudiDict::RelationWeighted2DDict<LHCb::Particle,LHCb::MCParticle,double>	_26 ;
        GaudiDict::RelationWeighted2DDict<LHCb::RecVertex,LHCb::GenCollision,std::pair<unsigned int,float> >	_27 ;
        GaudiDict::RelationWeighted2DDict<LHCb::RecVertex,LHCb::MCVertex,std::pair<unsigned int,float> >	_28 ;
        GaudiDict::RelationWeighted2DDict<LHCb::Track,LHCb::CaloCluster,float>	_29 ;
        GaudiDict::RelationWeighted2DDict<LHCb::Track,LHCb::CaloHypo,float>	_30 ;
        GaudiDict::RelationWeighted2DDict<LHCb::Track,LHCb::MCParticle,double>	_31 ;
  };
} // end of namespace Dict 

// ====================================================================
// The END ============================================================
// ====================================================================
