// $Id: CaloPIDs_load.cpp,v 1.3 2004-07-31 13:22:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================

/** @file
 *
 *  Mandatory file for dynamic loading of shared library
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-03-13
 */

DECLARE_FACTORY_ENTRIES(CaloPIDs) 
{
  
  //  
  DECLARE_ALGORITHM   ( CaloTrack2EstimatorAlg              ) ;
  DECLARE_ALGORITHM   ( CaloTrack2IdAlg                     ) ;
  DECLARE_TOOL        ( CaloTrackPrsEval                    ) ;
  DECLARE_TOOL        ( CaloTrackHcalEval                   ) ;
  DECLARE_TOOL        ( CaloTrackEcalEval                   ) ;
  DECLARE_TOOL        ( CaloTrackEval                       ) ;
  DECLARE_TOOL        ( CaloPhotonEstimatorTool             ) ;

  DECLARE_ALGORITHM   ( CaloPIDsData                        ) ;
};


// ============================================================================
// The END 
// ============================================================================
