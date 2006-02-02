// $Id: selection.h,v 1.1 2006-02-02 15:09:15 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef EVENTASSOC_EVENTASSOC_H 
#define EVENTASSOC_EVENTASSOC_H 1
// ============================================================================
// Incldue files 
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/RelationsDict.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/L0CaloCandidate.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
#include "Event/FlavourTag.h"
#include "Event/Vertex.h"
#include "Event/Track.h"
// ============================================================================

#include "Event/Particle.h"
#include "Event/Vertex.h"

#include "Relations/RelationsDict.h"

namespace EventAssoc
{
  struct __Instantiations 
  {    
    GaudiDict::Relation1DDict<
      LHCb::Track,float>                                    _1 ;
    GaudiDict::Relation2DDict<
      LHCb::Particle,LHCb::MCParticle>                      _2 ;
    GaudiDict::Relation2DDict<
      LHCb::Particle,LHCb::FlavourTag>                      _3 ;
    GaudiDict::Relation2DDict<
      LHCb::Particle,LHCb::Vertex>                          _4 ;
    GaudiDict::RelationWeighted2DDict<
      LHCb::CaloHypo,LHCb::Track,float>                     _5 ;
    GaudiDict::RelationWeighted2DDict<
      LHCb::CaloCluster,LHCb::MCParticle,float>             _6 ;
    GaudiDict::RelationWeighted2DDict<
      LHCb::ProtoParticle,LHCb::MCParticle, double>         _7 ;
    GaudiDict::RelationWeighted2DDict<
      LHCb::Particle,LHCb::MCParticle, double>              _8 ;
    GaudiDict::RelationWeighted2DDict<
      LHCb::Particle,LHCb::Vertex,float>                    _9 ;
  } ;
  
} ;  

// ============================================================================
// The END 
// ============================================================================
#endif // DICT_EVENTASSOC_H
// ============================================================================

