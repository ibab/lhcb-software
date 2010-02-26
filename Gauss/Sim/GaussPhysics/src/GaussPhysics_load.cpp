// $Id: GaussPhysics_load.cpp,v 1.17 2010-02-26 10:56:34 marcocle Exp $
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
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
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

// The DECLARE_NAMED_TOOL_FACTORY macro should be added to Gaudi
#ifndef DECLARE_NAMED_TOOL_FACTORY
#define DECLARE_NAMED_TOOL_FACTORY(x,n) PLUGINSVC_FACTORY_WITH_ID(x, std::string(#n), IAlgTool*(std::string, std::string, const IInterface*))
#endif
// Maybe this one should be added to Gaudi too
#define DECLARE_TYPEDEF_TOOL_FACTORY(x) DECLARE_NAMED_TOOL_FACTORY(x,x)

// Declaration of the External Physics list Factories
typedef GiGaExtPhysics< G4DecayPhysics > DecayFactory;
DECLARE_TOOL_FACTORY( DecayFactory );

typedef GiGaExtPhysics< G4EmStandardPhysics_option1 > EmStdOpt1PhysFactory;
DECLARE_TOOL_FACTORY( EmStdOpt1PhysFactory );
typedef GiGaExtPhysics< G4EmStandardPhysics_option2 > EmStdOpt2PhysFactory;
DECLARE_TOOL_FACTORY( EmStdOpt2PhysFactory );
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

// =========== Specialized extensions to GiGaExtPhysics ===========
template <>
class GiGaExtPhysicsExtender<G4DecayPhysics> {
public:
  inline void addPropertiesTo(AlgTool */*tool*/) {
    // No specific properties
  }
  inline G4DecayPhysics *newInstance(const std::string &name, int verbosity) const {
    return new G4DecayPhysics(name, verbosity);
  }
};

template <>
class GiGaExtPhysicsExtender<G4EmStandardPhysics_option1> {
public:
  inline void addPropertiesTo(AlgTool */*tool*/) {
    // No specific properties
  }
  inline G4EmStandardPhysics_option1 *newInstance(const std::string &name, int verbosity) const {
    return new G4EmStandardPhysics_option1(verbosity, name);
  }
};

template <>
class GiGaExtPhysicsExtender<G4QStoppingPhysics> {
public:
  inline void addPropertiesTo(AlgTool *tool) {
    tool->declareProperty("UseMuonMinusCapture", m_useMuonMinusCapture = true,
                          "Parameter 'UseMuonMinusCapture' for the constructor of G4QStoppingPhysics");
  }
  inline G4QStoppingPhysics *newInstance(const std::string &name, int verbosity) const {
    return new G4QStoppingPhysics(name, verbosity, m_useMuonMinusCapture);
  }
private:
  bool m_useMuonMinusCapture;
};

DECLARE_FACTORY_ENTRIES(GaussPhysics) { 
};

