// $Id: GaussCalo_load.cpp,v 1.3 2003-07-07 16:09:39 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#define GAUSSCALO_GAUSSCALO_LOAD_CPP 1 
// ============================================================================
// include 
// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// GiGa 
#include "GiGa/GiGaMACROs.h"

/** @file 
 *  The mandatory file for declaration of component library entries 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-09-26
 */

void GaussCalo_load() 
{ 
  /// Sensitive detectors
  DECLARE_GiGaFactory     ( EcalSensDet          ) ;
  DECLARE_GiGaFactory     ( HcalSensDet          ) ;
  DECLARE_GiGaFactory     ( SpdPrsSensDet        ) ;
  
  /// Converters 
  DECLARE_CONVERTER       ( GiGaCaloHitCnv       ) ;  
  DECLARE_CONVERTER       ( GaussSensPlaneHitCnv ) ;  
};
// ============================================================================


// ============================================================================
extern "C" void GaussCalo_loadRef() { GaussCalo_load(); } ; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
