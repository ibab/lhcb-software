// $Id: CaloAssociators_load.cpp,v 1.8 2003-04-02 14:46:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
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
// LHCbKernel
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

IMPLEMENT_RelationW2D( CaloCluster   , MCParticle , float );
IMPLEMENT_RelationW1D( CaloCluster   , MCParticle , float );
IMPLEMENT_WAssociator( CaloCluster   , MCParticle , float );


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

DECLARE_FACTORY_ENTRIES(CaloAssociators) 
{
  
  DECLARE_RelationW1D ( CaloCluster   , MCParticle , float ) ;
  DECLARE_RelationW2D ( CaloCluster   , MCParticle , float ) ;
  DECLARE_WAssociator ( CaloCluster   , MCParticle , float ) ;

  DECLARE_ALGORITHM   ( CaloClustersMCTruthAlg             ) ;
  DECLARE_ALGORITHM   ( CaloClustersMCTruth2Alg            ) ;
  DECLARE_ALGORITHM   ( CaloClustersMCTruth3Alg            ) ;
  DECLARE_ALGORITHM   ( CaloClustersMCTruth4Alg            ) ;
  DECLARE_ALGORITHM   ( CaloClustersMCTruth5Alg            ) ;
  
};

// ============================================================================
// The END 
// ============================================================================
