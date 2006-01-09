// $Id: G4BcMesonPlus.cpp,v 1.2 2006-01-09 20:52:22 robbep Exp $

#include <fstream>
#include <iomanip>

#include "G4BcMesonPlus.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      BcMesonPlus                        ###
// ######################################################################

G4BcMesonPlus * G4BcMesonPlus::theInstance = 0 ;

G4BcMesonPlus * G4BcMesonPlus::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "Bc+";
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
      new G4ParticleDefinition( name , 6.4*GeV , 1e-9*MeV , +1.*eplus ,
                                0,              -1,             0,
                                0,              0,             0,
                                "meson", 0,  0, 541,
                                false,     0.5e-3*ns,          NULL,
                                false, "Bc" );
  }
  theInstance = reinterpret_cast<G4BcMesonPlus*>(anInstance);
  return theInstance;
}

G4BcMesonPlus * G4BcMesonPlus::BcMesonPlusDefinition() {
  return Definition( ) ;
}

G4BcMesonPlus * G4BcMesonPlus::BcMesonPlus() {
  return Definition( ) ;
}
