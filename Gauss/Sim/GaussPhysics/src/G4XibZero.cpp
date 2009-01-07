// $Id: G4XibZero.cpp,v 1.1 2009-01-07 14:04:24 robbep Exp $

#include "G4XibZero.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      XibZero                        ###
// ######################################################################

G4XibZero * G4XibZero::theInstance = 0 ;

G4XibZero * G4XibZero::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "xi_b0";
  // search in particle table
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
      new G4ParticleDefinition( name , 5.840*GeV , 5.e-10*MeV , 0. ,
                                1,              +1,             0,
                                0,              0,             0,
                                "baryon", 0,  1, 5232,
                                false,     1.36e-3*ns,          NULL,
                                false, "xi_b" );
  }
  theInstance = reinterpret_cast<G4XibZero*>(anInstance);
  return theInstance;
}

G4XibZero * G4XibZero::XibZeroDefinition() {
  return Definition( ) ;
}

G4XibZero * G4XibZero::XibZero() {
  return Definition( ) ;
}
