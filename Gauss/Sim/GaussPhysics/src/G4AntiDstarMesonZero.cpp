// $Id: G4AntiDstarMesonZero.cpp,v 1.2 2006-01-09 20:52:22 robbep Exp $

#include <fstream>
#include <iomanip>

#include "G4AntiDstarMesonZero.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      AntiDstarMesonZero                        ###
// ######################################################################

G4AntiDstarMesonZero * G4AntiDstarMesonZero::theInstance = 0 ;

G4AntiDstarMesonZero * G4AntiDstarMesonZero::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "anti_D_star0";
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
      new G4ParticleDefinition( name , 2.0067*GeV , 0.0066*MeV , 0. ,
                                2,              -1,             0,
                                1,              +1,             0,
                                "meson", 0,  0, -423,
                                false,     0.0*ns,          NULL,
                                true , "D" );
  }
  theInstance = reinterpret_cast<G4AntiDstarMesonZero*>(anInstance);
  return theInstance;
}

G4AntiDstarMesonZero * G4AntiDstarMesonZero::AntiDstarMesonZeroDefinition() {
  return Definition( ) ;
}

G4AntiDstarMesonZero * G4AntiDstarMesonZero::AntiDstarMesonZero() {
  return Definition( ) ;
}
