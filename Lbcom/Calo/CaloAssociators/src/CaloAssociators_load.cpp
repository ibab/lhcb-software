// $Id: CaloAssociators_load.cpp,v 1.6 2002-09-04 15:16:14 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/06/26 19:03:56  ibelyaev
//  add one more new algorithm for MCTruth
//
// Revision 1.4  2002/06/13 12:28:47  ibelyaev
//  new options and new algorithm
//
// ============================================================================
// Include files
// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
// Event
#include "Event/KeyedObject.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// CaloEvent/Event 
#include "Event/CaloCluster.h"
// PhysEvent/Event
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
// LHCbKernel
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

IMPLEMENT_RelationW2D( CaloCluster   , MCParticle , float );
IMPLEMENT_RelationW1D( CaloCluster   , MCParticle , float );
IMPLEMENT_WAssociator( CaloCluster   , MCParticle , float );

IMPLEMENT_RelationW2D( ProtoParticle , MCParticle , float );
IMPLEMENT_RelationW1D( ProtoParticle , MCParticle , float );
IMPLEMENT_WAssociator( ProtoParticle , MCParticle , float );

IMPLEMENT_RelationW2D( Particle      , MCParticle , float );
IMPLEMENT_RelationW1D( Particle      , MCParticle , float );
IMPLEMENT_WAssociator( Particle      , MCParticle , float );

/** @file CaloAssociators_load.cpp 
 * 
 *  Mandatory file to IMPLEMENT and DECLARE all factories for 
 *  all generic relation tables and associator tools 
 *
 *  @see Relation1D
 *  @see RelationWeighted1D
 *  @see Relation2D
 *  @see RelationWeighted2D
 *  @see Associator
 *  @see AssociatorWeighted
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08 Apr 2002 
 */

DECLARE_FACTORY_ENTRIES(CaloAssociators) {
  
  DECLARE_RelationW1D( CaloCluster   , MCParticle , float ) ;
  DECLARE_RelationW2D( CaloCluster   , MCParticle , float ) ;
  DECLARE_WAssociator( CaloCluster   , MCParticle , float ) ;
  
  DECLARE_RelationW1D( ProtoParticle , MCParticle , float ) ;
  DECLARE_RelationW2D( ProtoParticle , MCParticle , float ) ;
  DECLARE_WAssociator( ProtoParticle , MCParticle , float ) ;
  
  DECLARE_RelationW1D( Particle      , MCParticle , float ) ;
  DECLARE_RelationW2D( Particle      , MCParticle , float ) ;
  DECLARE_WAssociator( Particle      , MCParticle , float ) ;
  
  DECLARE_ALGORITHM  ( CaloClustersMCTruthAlg             ) ;
  DECLARE_ALGORITHM  ( CaloClustersMCTruth2Alg            ) ;
  DECLARE_ALGORITHM  ( CaloClustersMCTruth3Alg            ) ;
  
  DECLARE_ALGORITHM  ( NeutralPPMCTruthAlg                ) ;
  
};

// ============================================================================
// The END 
// ============================================================================
