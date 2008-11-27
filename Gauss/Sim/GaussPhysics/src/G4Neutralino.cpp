// $Id: G4Neutralino.cpp,v 1.4 2008-11-27 20:18:13 robbep Exp $

#include <fstream>
#include <iomanip>

#include "G4Neutralino.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                       Neutralino                               ###
// ######################################################################

G4Neutralino * G4Neutralino::theInstance = 0 ;

G4Neutralino * G4Neutralino::Definition()
{

  if (theInstance !=0) return theInstance;
  const G4String name = "s_chi_0_1";
  // search in particle table]
  G4ParticleTable* pTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* anInstance = pTable->FindParticle(name);
  if (anInstance ==0)
  {
  // create particle
  //
  //    Arguments for constructor are as follows
  //               name             mass          width         charge
  //             2*spin           parity  C-conjugation
  //          2*Isospin       2*Isospin3       G-parity
  //               type    lepton number  baryon number   PDG encoding
  //             stable         lifetime    decay table
  //             shortlived      subType    anti_encoding
    anInstance = 
      new G4ParticleDefinition( name , 48.00*GeV, 3.12e-13*GeV, 0., 
                                1,              0,             0,
                                0,              0,             0,
                                "supersymmetric", 0,  0,  1000022,
                                false,     2.12e-3*ns,      NULL,
                                false, "Neutralino" );
  }
  theInstance = reinterpret_cast<G4Neutralino*>(anInstance);
  return theInstance;
}

G4Neutralino*  G4Neutralino::NeutralinoDefinition(){return Definition();}
G4Neutralino*  G4Neutralino::Neutralino(){return Definition();}

