// $Id: G4XiccStarPlus.cpp,v 1.1 2009-01-07 14:04:24 robbep Exp $

#include "G4XiccStarPlus.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      XiccStarPlus                        ###
// ######################################################################

G4XiccStarPlus * G4XiccStarPlus::theInstance = 0 ;

G4XiccStarPlus * G4XiccStarPlus::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "xi_ccstar+";
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
      new G4ParticleDefinition( name , 3.65648*GeV , 5.e-10*MeV ,  +1.*eplus ,
                                3,              +1,             0,
                                0,              0,             0,
                                "baryon", 0,  1, 4414,
                                false,     0.326e-3*ns,          NULL,
                                false, "xi_cc" );
  }
  theInstance = reinterpret_cast<G4XiccStarPlus*>(anInstance);
  return theInstance;
}

G4XiccStarPlus * G4XiccStarPlus::XiccStarPlusDefinition() {
  return Definition( ) ;
}

G4XiccStarPlus * G4XiccStarPlus::XiccStarPlus() {
  return Definition( ) ;
}
