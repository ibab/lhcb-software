// $Id: G4Neutralino.cpp,v 1.1 2008-05-07 08:34:30 gcorti Exp $
// For simplicity, neutralinos are constructed from mesons.
// For a correct implementation, see
///atlas/Simulation/G4Extensions/Gauginos/src/G4Neutralino.cxx

#include <fstream>
#include <iomanip>

#include "G4Neutralino.h"

#include "G4DecayTable.hh" 

// ######################################################################
// ###                       Neutralino                               ###
// ######################################################################

G4Neutralino::G4Neutralino(
       const G4String&     aName,        G4double            mass,
       G4double            width,        G4double            charge,   
       G4int               iSpin,        G4int               iParity,    
       G4int               iConjugation, G4int               iIsospin,   
       G4int               iIsospin3,    G4int               gParity,
       const G4String&     pType,        G4int               lepton,      
       G4int               baryon,       G4int               encoding,
       G4bool              stable,       G4double            lifetime,
       G4DecayTable        *decaytable )
//  : G4VMeson( aName,mass,width,charge,iSpin,iParity,
//              iConjugation,iIsospin,iIsospin3,gParity,pType,
//              lepton,baryon,encoding,stable,lifetime,decaytable )
  : G4ParticleDefinition( aName,mass,width,charge,iSpin,iParity,
			  iConjugation,iIsospin,iIsospin3,gParity,pType,
			  lepton,baryon,encoding,stable,lifetime,decaytable )
{   
  SetParticleSubType("Neutralino");
}

// ......................................................................
// ...                 static member definitions                      ...
// ......................................................................
//     
//    Arguments for constructor are as follows
//               name             mass          width         charge
//             2*spin           parity  C-conjugation
//          2*Isospin       2*Isospin3       G-parity
//               type    lepton number  baryon number   PDG encoding
//             stable         lifetime    decay table 

// 
G4Neutralino G4Neutralino::theNeutralino(
	      "s_chi_0_1",     48.00*GeV,     3.12e-13*GeV,      0., 
		    1,               0,            0,          
		    0,               0,            0,             
	    "supersymmetric",        0,            0,      1000022,
		false,          2.12e-3*ns,       NULL
);

G4Neutralino*  G4Neutralino::NeutralinoDefinition(){return &theNeutralino;}
G4Neutralino*  G4Neutralino::Neutralino(){return &theNeutralino;}

