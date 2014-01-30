// $

#include "G4AntiXibcMinus.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      AntiXibcMinus                        ###
// ######################################################################

G4AntiXibcMinus * G4AntiXibcMinus::theInstance = 0 ;

G4AntiXibcMinus * G4AntiXibcMinus::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "anti-xi_bc-";
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
      new G4ParticleDefinition( name , 6.9*GeV , 5.1e-10*MeV ,  -1.*eplus ,
                                3,              +1,             0,
                                1,              -1,             0,
                                "baryon", 0,  1, -5242,
                                false,     0.5e-3*ns,          NULL,
                                false, "xi_bc" );
  }
  theInstance = reinterpret_cast<G4AntiXibcMinus*>(anInstance);
  return theInstance;
}

G4AntiXibcMinus * G4AntiXibcMinus::AntiXibcMinusDefinition() {
  return Definition( ) ;
}

G4AntiXibcMinus * G4AntiXibcMinus::AntiXibcMinus() {
  return Definition( ) ;
}
