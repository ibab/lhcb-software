// $Id: G4Etac.cpp,v 1.1 2004-04-29 14:57:47 gcorti Exp $
#include <fstream>
#include <iomanip>

#include "G4Etac.h"

#include "G4DecayTable.hh" 

// ######################################################################
// ###                          Eta_c                                 ###
// ######################################################################

G4Etac::G4Etac(
       const G4String&     aName,        G4double            mass,
       G4double            width,        G4double            charge,   
       G4int               iSpin,        G4int               iParity,    
       G4int               iConjugation, G4int               iIsospin,   
       G4int               iIsospin3,    G4int               gParity,
       const G4String&     pType,        G4int               lepton,      
       G4int               baryon,       G4int               encoding,
       G4bool              stable,       G4double            lifetime,
       G4DecayTable        *decaytable )
 : G4VMeson( aName,mass,width,charge,iSpin,iParity,
             iConjugation,iIsospin,iIsospin3,gParity,pType,
             lepton,baryon,encoding,stable,lifetime,decaytable )
{
  SetParticleSubType("eta_c");
  // Anti-particle of eta_c is eta_c itself  
  SetAntiPDGEncoding(encoding);


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

// In this version, charged pions are set to stable
G4Etac G4Etac::theEtac(
	      "eta_c",     2.979700*GeV,     16.0*MeV,          0., 
		    0,              -1,            1,          
		    0,               0,            1,             
	      "meson",               0,             0,         441,
		false,          0.0*ns,          NULL
);

G4Etac*  G4Etac::EtacDefinition(){return &theEtac;}
G4Etac*  G4Etac::Etac(){return &theEtac;}

