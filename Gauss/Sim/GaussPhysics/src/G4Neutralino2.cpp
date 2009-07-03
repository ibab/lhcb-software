
#include <fstream>
#include <iomanip>

#include "G4Neutralino2.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                       Neutralino2                               ###
// ######################################################################

G4Neutralino2 * G4Neutralino2::theInstance = 0 ;

G4Neutralino2 * G4Neutralino2::Definition()
{

  if (theInstance !=0) return theInstance;
  const G4String name = "s_chi_0_2";
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
      new G4ParticleDefinition( name , 139.72*GeV, 5.3e-06*GeV, 0., 
                                1,              0,             0,
                                0,              0,             0,
                                "supersymmetric", 0,  0,  1000023,
                                false,     1.24e-10*ns,      NULL,
                                false, "Neutralino2" );
  }
  theInstance = reinterpret_cast<G4Neutralino2*>(anInstance);
  return theInstance;
}

G4Neutralino2*  G4Neutralino2::Neutralino2Definition(){return Definition();}
G4Neutralino2*  G4Neutralino2::Neutralino2(){return Definition();}

