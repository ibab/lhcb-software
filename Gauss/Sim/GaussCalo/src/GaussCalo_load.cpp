// $Id: GaussCalo_load.cpp,v 1.8 2007-01-12 15:24:47 ranjard Exp $ 
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h" 

// File mandatory for declaration of component library entries
// 
// 2007-01-10 : Gloria Corti
// Use standard DECLARE_FACTORY_ENTRIES and replace GiGaFactory
// with Tool factory

DECLARE_FACTORY_ENTRIES(GaussCalo) {

  // Sensitive detectors
  DECLARE_TOOL( EcalSensDet );
  DECLARE_TOOL( HcalSensDet );
  DECLARE_TOOL( SpdPrsSensDet );
  DECLARE_TOOL( GaussSensPlaneDet );
  
   // Algorithms
  DECLARE_ALGORITHM( GetCaloHitsAlg );

}
