// $Id: GaussPhysics_load.cpp,v 1.7 2006-01-09 20:52:22 robbep Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2005/11/09 18:10:26  gcorti
// add QGSP_BERT_HP from G4LHCblists
//
// Revision 1.5  2005/10/25 18:59:47  gcorti
// new physics lists
//
// Revision 1.4  2004/02/18 13:52:54  ibelyaev
//  new version with usage of 'external' physics lists
// 
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
#include "G4hadlists/G4EmStandardPhysics.hh"
IMPLEMENT_ExtPhysics( G4EmStandardPhysics          ) ;
#include "G4hadlists/G4EmExtraPhysics.hh"
IMPLEMENT_ExtPhysics( G4EmExtraPhysics       ) ;
//#include "G4hadlists/GeneralPhysics.hh"
//IMPLEMENT_ExtPhysics( GeneralPhysics     ) ;
#include "G4hadlists/G4IonPhysics.hh"
IMPLEMENT_ExtPhysics( G4IonPhysics         ) ;
//#include "G4hadlists/MuonPhysics.hh"
//IMPLEMENT_ExtPhysics( MuonPhysics        ) ;

#include "G4hadlists/HadronPhysicsLHEP.hh"
IMPLEMENT_ExtPhysics( HadronPhysicsLHEP    ) ;
//#include "G4hadlists/HadronPhysicsLHEP_GN.hh"
//IMPLEMENT_ExtPhysics( HadronPhysicsLHEP_GN ) ;
//#include "G4hadlists/HadronPhysicsLHEP_HP.hh"
//IMPLEMENT_ExtPhysics( HadronPhysicsLHEP_HP ) ;
#include "G4hadlists/HadronPhysicsLHEP_BERT_HP.hh"
IMPLEMENT_ExtPhysics( HadronPhysicsLHEP_BERT_HP ) ;

#include "G4hadlists/HadronPhysicsQGSP.hh"
IMPLEMENT_ExtPhysics( HadronPhysicsQGSP    ) ;
#include "G4hadlists/HadronPhysicsQGSP_HP.hh"
IMPLEMENT_ExtPhysics( HadronPhysicsQGSP_HP ) ;

//#include "G4LHCblists/HadronPhysicsQGSP_BERT_HP.hh"
#include "G4hadlists/HadronPhysicsQGSP_BERT_HP.hh"
IMPLEMENT_ExtPhysics( HadronPhysicsQGSP_BERT_HP ) ;

void GaussPhysics_load() 
{ 
  /// Physics Lists 
  //  DECLARE_GiGaFactory     (    GeneralPhysics          ) ;
  DECLARE_GiGaFactory     (    G4EmStandardPhysics               ) ;
  DECLARE_GiGaFactory     (    G4EmExtraPhysics            ) ;
  DECLARE_GiGaFactory     (    G4IonPhysics              ) ;
  //  DECLARE_GiGaFactory     (    MuonPhysics             ) ;
  DECLARE_GiGaFactory     (    HadronPhysicsLHEP       ) ;
  //  DECLARE_GiGaFactory     (    HadronPhysicsLHEP_GN    ) ;
  //  DECLARE_GiGaFactory     (    HadronPhysicsLHEP_HP    ) ;
  DECLARE_GiGaFactory     (    HadronPhysicsLHEP_BERT_HP  ) ;
  DECLARE_GiGaFactory     (    HadronPhysicsQGSP       ) ;
  DECLARE_GiGaFactory     (    HadronPhysicsQGSP_HP    ) ;
  DECLARE_GiGaFactory     (    HadronPhysicsQGSP_BERT_HP  ) ;

};
// ============================================================================

// ============================================================================
extern "C" void GaussPhysics_loadRef() { GaussPhysics_load(); } ; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
