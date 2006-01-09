// $Id: G4DstarMesonPlus.cpp,v 1.2 2006-01-09 20:52:22 robbep Exp $

#include <fstream>
#include <iomanip>

#include "G4DstarMesonPlus.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      BcMesonMinus                        ###
// ######################################################################

G4DstarMesonPlus * G4DstarMesonPlus::theInstance = 0 ;

G4DstarMesonPlus * G4DstarMesonPlus::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "D_star+";
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
      new G4ParticleDefinition( name , 2.0100*GeV , 0.096*MeV , +1.*eplus ,
                                2,              -1,             0,
                                1,              +1,             0,
                                "meson", 0,  0, 413,
                                false,     0.0*ns,          NULL,
                                true, "D" );
  }
  theInstance = reinterpret_cast<G4DstarMesonPlus*>(anInstance);
  return theInstance;
}

G4DstarMesonPlus * G4DstarMesonPlus::DstarMesonPlusDefinition() {
  return Definition( ) ;
}

G4DstarMesonPlus * G4DstarMesonPlus::DstarMesonPlus() {
  return Definition( ) ;
}
