// $Id: G4DsstarMesonPlus.cpp,v 1.2 2006-01-09 20:52:22 robbep Exp $

#include <fstream>
#include <iomanip>

#include "G4DsstarMesonPlus.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      BcMesonMinus                        ###
// ######################################################################

G4DsstarMesonPlus * G4DsstarMesonPlus::theInstance = 0 ;

G4DsstarMesonPlus * G4DsstarMesonPlus::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "Ds_star+";
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
      new G4ParticleDefinition( name , 2.1124*GeV , 0.0066*MeV , +1.*eplus ,
                                2,              -1,             0,
                                0,              0,             0,
                                "meson", 0,  0, 433,
                                false,     0.0*ns,          NULL,
                                true, "Ds" );
  }
  theInstance = reinterpret_cast<G4DsstarMesonPlus*>(anInstance);
  return theInstance;
}

G4DsstarMesonPlus * G4DsstarMesonPlus::DsstarMesonPlusDefinition() {
  return Definition( ) ;
}

G4DsstarMesonPlus * G4DsstarMesonPlus::DsstarMesonPlus() {
  return Definition( ) ;
}
