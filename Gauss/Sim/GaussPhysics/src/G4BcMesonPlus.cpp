// $Id: G4BcMesonPlus.cpp,v 1.1 2004-04-29 14:57:46 gcorti Exp $

#include <fstream>
#include <iomanip>

#include "G4BcMesonPlus.h"

#include "G4DecayTable.hh"

// ######################################################################
// ###                          BcMesonPlus                           ###
// ######################################################################

G4BcMesonPlus::G4BcMesonPlus(
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
   SetParticleSubType("Bc");
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
G4BcMesonPlus G4BcMesonPlus::theBcMesonPlus
(
 "Bc+",      6.4*GeV,   1.e-9*MeV,          +1.*eplus, 
 0,              -1,             0,          
 0,               0,             0,             
 "meson",               0,             0,         541,
 false,      0.5e-3*ns,          NULL
);

G4BcMesonPlus*  G4BcMesonPlus::BcMesonPlusDefinition(){return &theBcMesonPlus;}
G4BcMesonPlus*  G4BcMesonPlus::BcMesonPlus(){return &theBcMesonPlus;}

