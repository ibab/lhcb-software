// $Id: GaussPhysics_load.cpp,v 1.13 2008-04-26 22:01:57 robbep Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GiGa/GiGaExtPhysics.h"


/** @file 
 *  The mandatory file for declaration of component library entries 
 *  @author Witold Pokorsky
 *  @author Vanya Belyaev
 *  @author Gloria Corti, port to Gaudi v19
 *  @date 2002-09-26, last modified 2007-01-19
 */

// Geant4 physics lists
#include "G4DecayPhysics.hh"

#// EM physics 
#include "G4EmStandardPhysics71.hh"
#include "G4EmStandardPhysics72.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"

// Ion and hadrons
#include "G4IonPhysics.hh"
#include "G4QStoppingPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4NeutronTrackingCut.hh"

// LHEP hadrons
#include "HadronPhysicsLHEP.hh"

// QGSP hadrons
#include "HadronPhysicsQGSP.hh"
#include "HadronPhysicsQGSP_BERT_HP.hh"

// QGSC hadrons
#include "HadronPhysicsQGSC.hh"
#include "HadronPhysicsQGSC_EFLOW.hh"


// Declaration of the External Physics list Factories
typedef GiGaExtPhysics< G4DecayPhysics > DecayFactory;
DECLARE_TOOL_FACTORY( DecayFactory );

typedef GiGaExtPhysics< G4EmStandardPhysics71 > EmStd71PhysFactory;
DECLARE_TOOL_FACTORY( EmStd71PhysFactory );
typedef GiGaExtPhysics< G4EmStandardPhysics72 > EmStd72PhysFactory;
DECLARE_TOOL_FACTORY( EmStd72PhysFactory );
typedef GiGaExtPhysics< G4EmStandardPhysics > EmStdPhysFactory;
DECLARE_TOOL_FACTORY( EmStdPhysFactory );

typedef GiGaExtPhysics< G4EmExtraPhysics > EmExtraPhysFactory;
DECLARE_TOOL_FACTORY( EmExtraPhysFactory );

typedef GiGaExtPhysics< G4IonPhysics > IonPhysFactory;
DECLARE_TOOL_FACTORY( IonPhysFactory );
typedef GiGaExtPhysics< G4QStoppingPhysics > QStopPhysFactory;
DECLARE_TOOL_FACTORY( QStopPhysFactory );
typedef GiGaExtPhysics< G4HadronElasticPhysics > HadElPhysFactory;
DECLARE_TOOL_FACTORY( HadElPhysFactory );
typedef GiGaExtPhysics< G4NeutronTrackingCut > NeuTrkCutFactory;
DECLARE_TOOL_FACTORY( NeuTrkCutFactory );

typedef GiGaExtPhysics< HadronPhysicsLHEP > HadPhysLHEPFactory;
DECLARE_TOOL_FACTORY( HadPhysLHEPFactory );

typedef GiGaExtPhysics< HadronPhysicsQGSP > HadPhysQGSPFactory;
DECLARE_TOOL_FACTORY( HadPhysQGSPFactory );
typedef GiGaExtPhysics< HadronPhysicsQGSP_BERT_HP > HadPhysQGSP_BERT_HPFactory;
DECLARE_TOOL_FACTORY( HadPhysQGSP_BERT_HPFactory );

typedef GiGaExtPhysics< HadronPhysicsQGSC > HadPhysQGSCFactory;
DECLARE_TOOL_FACTORY( HadPhysQGSCFactory );
typedef GiGaExtPhysics< HadronPhysicsQGSC_EFLOW > HadPhysQGSC_EFLOWFactory;
DECLARE_TOOL_FACTORY( HadPhysQGSC_EFLOWFactory );

DECLARE_FACTORY_ENTRIES(GaussPhysics) { 
  DECLARE_TOOL( GiGaPhysUnknownParticles ) ;
};

