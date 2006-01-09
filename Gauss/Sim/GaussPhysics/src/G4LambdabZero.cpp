// $Id: G4LambdabZero.cpp,v 1.1 2006-01-09 20:52:07 robbep Exp $

#include <fstream>
#include <iomanip>

#include "G4LambdabZero.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      LambdabZero                        ###
// ######################################################################

G4LambdabZero * G4LambdabZero::theInstance = 0 ;

G4LambdabZero * G4LambdabZero::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "lambda_b0";
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
      new G4ParticleDefinition( name , 5.624*GeV , 5.e-10*MeV , 0. ,
                                1,              +1,             0,
                                0,              0,             0,
                                "baryon", 0,  1, 5122,
                                false,     1.2e-3*ns,          NULL,
                                false, "lambda_b" );
  }
  theInstance = reinterpret_cast<G4LambdabZero*>(anInstance);
  return theInstance;
}

G4LambdabZero * G4LambdabZero::LambdabZeroDefinition() {
  return Definition( ) ;
}

G4LambdabZero * G4LambdabZero::LambdabZero() {
  return Definition( ) ;
}
