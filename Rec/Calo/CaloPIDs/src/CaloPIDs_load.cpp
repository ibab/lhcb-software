// $Id: CaloPIDs_load.cpp,v 1.7 2006-06-26 08:35:49 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2006/06/18 18:35:27  ibelyaev
//  the firstcommmit for DC06 branch
//
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================
/** @file
 *  Mandatory file for implementation of compopnent library
 *  @author Vanya BELYAEV ibelyaev@phsyics.syr.edu
 *  @date 2006-05-27
 */
// ============================================================================
DECLARE_FACTORY_ENTRIES(CaloPIDs) 
{
  DECLARE_TOOL      ( InCaloAcceptance    ) ;
  //
  DECLARE_TOOL      ( InSpdAcceptance     ) ;
  DECLARE_TOOL      ( InPrsAcceptance     ) ;
  DECLARE_TOOL      ( InEcalAcceptance    ) ;
  DECLARE_TOOL      ( InHcalAcceptance    ) ;
  DECLARE_TOOL      ( InBremAcceptance    ) ;
  //
  //
  DECLARE_TOOL      ( CaloEnergyForTrack  ) ;
  //
  DECLARE_TOOL      ( SpdEnergyForTrack   ) ;
  DECLARE_TOOL      ( PrsEnergyForTrack   ) ;
  DECLARE_TOOL      ( EcalEnergyForTrack  ) ;
  DECLARE_TOOL      ( HcalEnergyForTrack  ) ;
  //
  //
  DECLARE_TOOL      ( CaloPhotonMatch     ) ;
  DECLARE_TOOL      ( CaloElectronMatch   ) ;
  DECLARE_TOOL      ( CaloBremMatch       ) ;
  //
  //
  DECLARE_ALGORITHM ( CaloTrack2IDAlg     ) ;
  //
  DECLARE_ALGORITHM ( Track2SpdEAlg       ) ;
  DECLARE_ALGORITHM ( Track2PrsEAlg       ) ;
  DECLARE_ALGORITHM ( Track2EcalEAlg      ) ;
  DECLARE_ALGORITHM ( Track2HcalEAlg      ) ;
  //
  //
  DECLARE_ALGORITHM ( PhotonMatchAlg      ) ;
  DECLARE_ALGORITHM ( ElectronMatchAlg    ) ;
  DECLARE_ALGORITHM ( BremMatchAlg        ) ;
  //
  //
  DECLARE_ALGORITHM ( InCaloAcceptanceAlg ) ;
  //
  DECLARE_ALGORITHM ( InEcalAcceptanceAlg ) ;
  DECLARE_ALGORITHM ( InHcalAcceptanceAlg ) ;
  DECLARE_ALGORITHM ( InBremAcceptanceAlg ) ;
  DECLARE_ALGORITHM ( InPrsAcceptanceAlg  ) ;
  DECLARE_ALGORITHM ( InSpdAcceptanceAlg  ) ;
  //
  //
  DECLARE_ALGORITHM ( CaloID2DLL          ) ;
  //
  DECLARE_ALGORITHM ( PrsPIDeAlg          ) ;
  DECLARE_ALGORITHM ( EcalPIDeAlg         ) ;
  DECLARE_ALGORITHM ( BremPIDeAlg         ) ;
  DECLARE_ALGORITHM ( HcalPIDeAlg         ) ;
  DECLARE_ALGORITHM ( EcalPIDmuAlg        ) ;
  DECLARE_ALGORITHM ( HcalPIDmuAlg        ) ;
  //
  //
  DECLARE_ALGORITHM ( CaloChi22ID         ) ;
  //
  DECLARE_ALGORITHM ( EcalChi22ID         ) ;
  DECLARE_ALGORITHM ( BremChi22ID         ) ;
  DECLARE_ALGORITHM ( ClusChi22ID         ) ;
};

// ============================================================================

// ============================================================================
// The END 
// ============================================================================
