// $Id: GaussPhysics_load.cpp,v 1.11 2007-03-19 12:09:37 gcorti Exp $
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
#include "HadronPhysicsLHEP_BERT_HP.hh"
#include "HadronPhysicsLHEP_EMV.hh"  // with Em 7.1

// QGSP hadrons
#include "HadronPhysicsQGSP.hh"
#include "HadronPhysicsQGSP_BERT_HP.hh"
#include "HadronPhysicsQGSP_EMV.hh"  // with Em 7.1 
#include "HadronPhysicsQGSP_EMX.hh"  // with Em 7.2 


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
typedef GiGaExtPhysics< HadronPhysicsLHEP_BERT_HP > HadPhysLHEP_BERT_HPFactory;
DECLARE_TOOL_FACTORY( HadPhysLHEP_BERT_HPFactory );
typedef GiGaExtPhysics< HadronPhysicsLHEP_EMV > HadPhysLHEP_EMVFactory;
DECLARE_TOOL_FACTORY( HadPhysLHEP_EMVFactory );

typedef GiGaExtPhysics< HadronPhysicsQGSP > HadPhysQGSPFactory;
DECLARE_TOOL_FACTORY( HadPhysQGSPFactory );
typedef GiGaExtPhysics< HadronPhysicsQGSP_BERT_HP > HadPhysQGSP_BERT_HPFactory;
DECLARE_TOOL_FACTORY( HadPhysQGSP_BERT_HPFactory );
typedef GiGaExtPhysics< HadronPhysicsQGSP_EMV > HadPhysQGSP_EMVFactory;
DECLARE_TOOL_FACTORY( HadPhysQGSP_EMVFactory );
typedef GiGaExtPhysics< HadronPhysicsQGSP_EMX > HadPhysQGSP_EMXFactory;
DECLARE_TOOL_FACTORY( HadPhysQGSP_EMXFactory );

DECLARE_FACTORY_ENTRIES(GaussPhysics) { 

};

