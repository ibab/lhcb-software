// $Id: CaloPIDs_load.cpp,v 1.2 2004-03-17 16:11:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2003/03/13 18:52:02  ibelyaev
// The first import of new package 
// 
// ============================================================================
// Include files
// LHCbKernel
#include "Relations/RelationMACROs.h"
// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h"
// Event/TrEvent 
#include "Event/TrStoredTrack.h"

/** @file
 *
 *  Mandatory file for dynamic loading of shared library
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-03-13
 */

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

/// implement the relation associator 
IMPLEMENT_Associator (               TrStoredTrack , float ) ;

DECLARE_FACTORY_ENTRIES(CaloPIDs) 
{
  
  DECLARE_Associator  ( TrStoredTrack , float               ) ;
  
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
