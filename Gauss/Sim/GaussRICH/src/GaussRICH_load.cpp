// $Id: GaussRICH_load.cpp,v 1.4 2007-01-12 15:31:55 ranjard Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h" 

//----------------------------------------------------------------------------
// The mandatory file for declaration of component library entries
//
// 2002-09-26 : Sajan Easo 
// 2007-01-11 : Gloria Corti, adapt to Gaudi v19 (also compatible with v18)
//----------------------------------------------------------------------------

DECLARE_FACTORY_ENTRIES( GaussRICH ) {
  
  /// Sensitive Detector 
  DECLARE_TOOL( RichSensDet );

  /// run action
  DECLARE_TOOL( RichG4RunAction );

  /// event action 
  DECLARE_TOOL( RichG4EventAction );
  
  /// physics constructors
  DECLARE_TOOL( GiGaPhysConstructorOp );

  /// algorithms
  DECLARE_ALGORITHM( GetMCRichHitsAlg );
  DECLARE_ALGORITHM( GetMCRichOpticalPhotonsAlg );
  DECLARE_ALGORITHM( GetMCRichSegmentsAlg );
  DECLARE_ALGORITHM( GetMCRichTracksAlg );

}

