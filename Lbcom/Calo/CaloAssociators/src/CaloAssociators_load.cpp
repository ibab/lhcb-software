// $Id: CaloAssociators_load.cpp,v 1.1.1.1 2002-04-08 14:27:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h"
// LHCbKernel
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"
// Event
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// CaloEvent/Event 
#include  "Event/CaloCluster.h"

IMPLEMENT_RelationW2D( CaloCluster , MCParticle , double );
IMPLEMENT_WAssociator( CaloCluster , MCParticle , double );

/** @file CaloAssociators_load.cpp 
 * 
 *  Mandatory file to IMPLEMENT and DECLARE all factories for 
 *  all generic relation tablles and associsator tools 
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
  
  DECLARE_RelationW2D( CaloCluster , MCParticle , double );
  DECLARE_WAssociator( CaloCluster , MCParticle , double );
  
};

// ============================================================================
// The END 
// ============================================================================
