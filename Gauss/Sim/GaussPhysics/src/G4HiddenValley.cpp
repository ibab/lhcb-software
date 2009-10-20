// $Id: G4HiddenValley.cpp,v 1.1 2009-10-20 07:31:23 marcin Exp $

#include <fstream>
#include <iomanip>

#include "G4HiddenValley.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      HiddenValley                        ###
// ######################################################################

G4HiddenValley * G4HiddenValley::theInstance = 0 ;

G4HiddenValley * G4HiddenValley::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "HVpion";
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
      new G4ParticleDefinition( name , 35.0*GeV , 5.e-10*MeV , 0. ,
                                1,              +1,             0,
                                0,              0,             0,
                                "baryon", 0,  1, 36,
                                false,     0.1*ns,          NULL,
                                false, "HVpion");
  }
  theInstance = reinterpret_cast<G4HiddenValley*>(anInstance);
  return theInstance;
}

G4HiddenValley * G4HiddenValley::HiddenValleyDefinition() {
  return Definition( ) ;
}

G4HiddenValley * G4HiddenValley::HiddenValley() {
  return Definition( ) ;
}
