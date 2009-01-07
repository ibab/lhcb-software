// $Id: G4AntiXibZero.cpp,v 1.1 2009-01-07 14:04:24 robbep Exp $

#include "G4AntiXibZero.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      AntiXibZero                        ###
// ######################################################################

G4AntiXibZero * G4AntiXibZero::theInstance = 0 ;

G4AntiXibZero * G4AntiXibZero::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "anti_xi_b0";
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
      new G4ParticleDefinition( name , 5.840*GeV , 5e-10*MeV , 0. ,
                                1,              +1,             0,
                                0,              0,             0,
                                "baryon", 0,  1, -5232,
                                false,     1.36e-3*ns,          NULL,
                                false, "xi_b" );
  }
  theInstance = reinterpret_cast<G4AntiXibZero*>(anInstance);
  return theInstance;
}

G4AntiXibZero * G4AntiXibZero::AntiXibZeroDefinition() {
  return Definition( ) ;
}

G4AntiXibZero * G4AntiXibZero::AntiXibZero() {
  return Definition( ) ;
}
