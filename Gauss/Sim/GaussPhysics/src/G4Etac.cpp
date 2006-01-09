// $Id: G4Etac.cpp,v 1.2 2006-01-09 20:52:22 robbep Exp $

#include <fstream>
#include <iomanip>

#include "G4Etac.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      Etac                        ###
// ######################################################################

G4Etac * G4Etac::theInstance = 0 ;

G4Etac * G4Etac::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "eta_c";
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
      new G4ParticleDefinition( name , 2.9797*GeV , 16*MeV , 0., 
                                0,              -1,             1,
                                0,              0,             1,
                                "meson", 0,  0, 441,
                                false,     0.*ns,          NULL,
                                true, "eta_c" );
  }
  theInstance = reinterpret_cast<G4Etac*>(anInstance);
  return theInstance;
}

G4Etac * G4Etac::EtacDefinition() {
  return Definition( ) ;
}

G4Etac * G4Etac::Etac() {
  return Definition( ) ;
}
