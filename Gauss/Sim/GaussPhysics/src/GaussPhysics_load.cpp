// $Id: GaussPhysics_load.cpp,v 1.4 2004-02-18 13:52:54 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#define GAUSSPHYSICS_GAUSSPHYSICS_LOAD_CPP 1 
// ============================================================================
// include 
// ============================================================================
// GaudiKernel 
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h" 
// ============================================================================
// GiGa 
// ============================================================================
#include "GiGa/GiGaExtPhysics.h"
// ============================================================================
#include "GiGa/GiGaMACROs.h"
// ============================================================================


/** @file 
 *  The mandatory file for declaration of component library entries 
 *  @author Witold Pokorsky Witold.Pokorsky@cern.ch 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-09-26
 */

// Packaging
#include "G4hadlists/EMPhysics.hh"
IMPLEMENT_ExtPhysics( EMPhysics          ) ;
#include "G4hadlists/EM_GNPhysics.hh"
IMPLEMENT_ExtPhysics( EM_GNPhysics       ) ;
#include "G4hadlists/GeneralPhysics.hh"
IMPLEMENT_ExtPhysics( GeneralPhysics     ) ;
#include "G4hadlists/IonPhysics.hh"
IMPLEMENT_ExtPhysics( IonPhysics         ) ;
#include "G4hadlists/MuonPhysics.hh"
IMPLEMENT_ExtPhysics( MuonPhysics        ) ;

#include "G4hadlists/HadronPhysicsLHEP.hh"
IMPLEMENT_ExtPhysics( HadronPhysicsLHEP    ) ;
#include "G4hadlists/HadronPhysicsLHEP_GN.hh"
IMPLEMENT_ExtPhysics( HadronPhysicsLHEP_GN ) ;

#include "G4hadlists/HadronPhysicsQGSP.hh"
IMPLEMENT_ExtPhysics( HadronPhysicsQGSP    ) ;

void GaussPhysics_load() 
{ 
  /// Physics Lists 
  DECLARE_GiGaFactory     (    GeneralPhysics          ) ;
  DECLARE_GiGaFactory     (    EMPhysics               ) ;
  DECLARE_GiGaFactory     (    EM_GNPhysics            ) ;
  DECLARE_GiGaFactory     (    IonPhysics              ) ;
  DECLARE_GiGaFactory     (    MuonPhysics             ) ;
  DECLARE_GiGaFactory     (    HadronPhysicsLHEP       ) ;
  DECLARE_GiGaFactory     (    HadronPhysicsLHEP_GN    ) ;
  DECLARE_GiGaFactory     (    HadronPhysicsQGSP       ) ;

};
// ============================================================================

// ============================================================================
extern "C" void GaussPhysics_loadRef() { GaussPhysics_load(); } ; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
