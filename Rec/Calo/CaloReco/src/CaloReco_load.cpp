// $Id: CaloReco_load.cpp,v 1.9 2004-06-03 06:33:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2004/03/19 16:03:20  cattanem
// add missing factories...
//
// Revision 1.7  2003/04/11 09:33:42  ibelyaev
//  add new E-,S- and L-corrections from Olivier Deschamps
//
// Revision 1.6  2003/03/13 18:47:50  ibelyaev
//  see ../doc/release.notes file
//
// ============================================================================
// Include files
// LHCbKernel
#include "Relations/RelationMACROs.h"
// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/SmartRef.h"
// Event/TrEvent 
#include "Event/TrStoredTrack.h"
// Event/CaloEvent 
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"

/** @file
 *  mandatory file for dynamic loading of CaloReco shared library.
 *  All 'loadable' components must be declard in this file 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-10-31 
 */

/// implement the relation associator
IMPLEMENT_WAssociator( CaloCluster , TrStoredTrack , float ) ;
/// implement the relation associator 
IMPLEMENT_WAssociator( CaloHypo    , TrStoredTrack , float ) ;

DECLARE_FACTORY_ENTRIES(CaloReco) 
{
  
  // From CaloAlgs package 
  
  DECLARE_ALGORITHM   ( CaloDeleteObjectsAlg                ) ;
  DECLARE_ALGORITHM   ( CaloDigitsFilterAlg                 ) ;
  DECLARE_ALGORITHM   ( CaloHypoAlg                         ) ;
  DECLARE_ALGORITHM   ( CaloHyposMerge                      ) ;
  
  DECLARE_ALGORITHM   ( CaloClusterCovarianceAlg            ) ;
  DECLARE_ALGORITHM   ( CaloSharedCellAlg                   ) ;
  DECLARE_ALGORITHM   ( CaloClusterisationAlg               ) ;
  DECLARE_ALGORITHM   ( CellularAutomaton                   ) ;

  // From CaloRec package
  DECLARE_ALGORITHM   ( CaloSinglePhotonAlg                 ) ;
  DECLARE_ALGORITHM   ( CaloMergedPi0Alg                    ) ;
  DECLARE_ALGORITHM   ( CaloElectronAlg                     ) ;
  
  DECLARE_ALGORITHM   ( CaloTrackMatchPhotonAlg             ) ;
  DECLARE_ALGORITHM   ( CaloCluster2TrackAlg                ) ;
  DECLARE_ALGORITHM   ( CaloHypo2TrackAlg                   ) ;
  
  DECLARE_WAssociator ( CaloCluster , TrStoredTrack , float ) ;  
  DECLARE_WAssociator ( CaloHypo    , TrStoredTrack , float ) ;
  
  DECLARE_TOOL        ( CaloSelectNeutralCluster            ) ;

  // from CaloTools package 
  DECLARE_TOOL        ( CaloClusterResetFractions           ) ;
  DECLARE_TOOL        ( ClusterCovarianceMatrixTool         ) ;
  DECLARE_TOOL        ( ClusterSpreadTool                   ) ;

  DECLARE_TOOL        ( SubClusterSelectorAll               ) ;
  DECLARE_TOOL        ( SubClusterSelector3x3               ) ;
  DECLARE_TOOL        ( SubClusterSelectorMax2x2            ) ;
  DECLARE_TOOL        ( SubClusterSelectorSwissCross        ) ;
  DECLARE_TOOL        ( SubClusterSelectorNeighbors         ) ;

  DECLARE_TOOL        ( CaloCorrASINH                       ) ;
  DECLARE_TOOL        ( CaloCorr2TANH                       ) ;
  
  DECLARE_TOOL        ( CaloSelector                        ) ;
  DECLARE_TOOL        ( CaloSelectorAND                     ) ;
  DECLARE_TOOL        ( CaloSelectorOR                      ) ;
  DECLARE_TOOL        ( CaloSelectorNOT                     ) ;
   
  DECLARE_TOOL        ( CaloClusterToolSequence             ) ;
  DECLARE_TOOL        ( CaloHypoToolSequence                ) ;
  
  DECLARE_TOOL        ( CaloExtraDigits                     ) ;

  DECLARE_TOOL        ( CaloSShape                          ) ;
  DECLARE_TOOL        ( CaloLCorrection                     ) ;

  DECLARE_TOOL        ( CaloPrsCorrection                   ) ;
  DECLARE_TOOL        ( CaloRemoveMomentum                  ) ;

  DECLARE_TOOL        ( CaloPhotonMomentumTool              ) ;

  DECLARE_TOOL        ( CaloClusterSOMSelector              ) ;

  DECLARE_TOOL        ( CaloSingleGammaTool                 ) ;
  
  DECLARE_TOOL        ( CaloLCorrectionSimple               ) ;

  DECLARE_TOOL        ( CaloSCorrectionSequence             ) ;
  DECLARE_TOOL        ( CaloSCorrectionCellCenter           ) ;
  DECLARE_TOOL        ( CaloSCorrectionBarycentre           ) ;
  DECLARE_TOOL        ( CaloSCorrectionArcSinh              ) ;
  DECLARE_TOOL        ( CaloSCorrectionLinear               ) ;
  DECLARE_TOOL        ( CaloSCorrectionDoubleTanh           ) ;
  DECLARE_TOOL        ( CaloSCorrectionComplex              ) ;
  DECLARE_TOOL        ( CaloSCorrectionFinal                ) ;

  // From CaloTrackMatch package 
  DECLARE_TOOL        ( CaloTrackMatchPhoton                ) ;
  DECLARE_TOOL        ( CaloTrackMatchElectron              ) ;
  DECLARE_TOOL        ( CaloTrackMatchBremm                 ) ;

  DECLARE_ALGORITHM   ( CaloHypoReShuffleAlg                ) ;

  DECLARE_TOOL        ( CaloNewECorrection                  ) ;
  DECLARE_TOOL        ( CaloNewSCorrection                  ) ;
  DECLARE_TOOL        ( CaloNewLCorrection                  ) ;
  DECLARE_TOOL        ( Calo04ECorrection                   ) ;
  DECLARE_TOOL        ( Calo04SCorrection                   ) ;
  DECLARE_TOOL        ( Calo04LCorrection                   ) ;

  DECLARE_TOOL ( CaloClusterCorrect3x3Position );
  
};


// ============================================================================
// TheEND 
// ============================================================================
