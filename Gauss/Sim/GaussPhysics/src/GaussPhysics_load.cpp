// $Id: GaussPhysics_load.cpp,v 1.9 2007-01-17 17:48:17 ranjard Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2006/07/21 08:02:36  ranjard
// v4r0 - use new physics lists from geant4 8.1
//
// Revision 1.7  2006/01/09 20:52:22  robbep
// Adapt to Geant4.8
//
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


/** @file 
 *  The mandatory file for declaration of component library entries 
 *  @author Witold Pokorsky Witold.Pokorsky@cern.ch 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-09-26
 */

// Packaging
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonPhysics.hh"

#include "HadronPhysicsLHEP.hh"
#include "HadronPhysicsLHEP_BERT_HP.hh"

#include "HadronPhysicsQGSP.hh"
#include "HadronPhysicsQGSP_HP.hh"

#include "HadronPhysicsQGSP_BERT_HP.hh"

DECLARE_FACTORY_ENTRIES(GaussPhysics) 
{ 
  /// Physics Lists 
  DECLARE_TOOL     (    G4EmStandardPhysics       ) ;
  DECLARE_TOOL     (    G4EmExtraPhysics          ) ;
  DECLARE_TOOL     (    G4IonPhysics              ) ;
  DECLARE_TOOL     (    HadronPhysicsLHEP         ) ;
  DECLARE_TOOL     (    HadronPhysicsLHEP_BERT_HP ) ;
  DECLARE_TOOL     (    HadronPhysicsQGSP         ) ;
  DECLARE_TOOL     (    HadronPhysicsQGSP_HP      ) ;
  DECLARE_TOOL     (    HadronPhysicsQGSP_BERT_HP ) ;

};

// ============================================================================
// The END 
// ============================================================================
