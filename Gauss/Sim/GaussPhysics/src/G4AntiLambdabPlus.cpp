// $Id: G4AntiLambdabPlus.cpp,v 1.1 2004-04-29 14:57:46 gcorti Exp $

#include <fstream>
#include <iomanip>

#include "G4AntiLambdabPlus.h"

#include "G4PhaseSpaceDecayChannel.hh"
#include "G4DecayTable.hh"

// ######################################################################
// ###                           AntiLambdabPlus                      ###
// ######################################################################

G4AntiLambdabPlus::G4AntiLambdabPlus(
       const G4String&     aName,        G4double            mass,
       G4double            width,        G4double            charge,   
       G4int               iSpin,        G4int               iParity,    
       G4int               iConjugation, G4int               iIsospin,   
       G4int               iIsospin3,    G4int               gParity,
       const G4String&     pType,        G4int               lepton,      
       G4int               baryon,       G4int               encoding,
       G4bool              stable,       G4double            lifetime,
       G4DecayTable        *decaytable )
 : G4VBaryon( aName,mass,width,charge,iSpin,iParity,
              iConjugation,iIsospin,iIsospin3,gParity,pType,
              lepton,baryon,encoding,stable,lifetime,decaytable )
{
  SetParticleSubType("lambda_b");
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

G4AntiLambdabPlus G4AntiLambdabPlus::theAntiLambdabPlus
(
 "anti_lambda_b+",      5.624*GeV,   5.e-10*MeV,   0.,
 1,              +1,             0,          
 0,               0,             0,             
 "baryon",               0,            +1,       -5122,
 false,     1.2e-3*ns,          NULL
);

G4AntiLambdabPlus* G4AntiLambdabPlus::AntiLambdabPlusDefinition()
{return &theAntiLambdabPlus;}
G4AntiLambdabPlus* G4AntiLambdabPlus::AntiLambdabPlus()
{return &theAntiLambdabPlus;}

