// $Id: G4AntiLambdabZero.cpp,v 1.1 2006-01-09 20:52:07 robbep Exp $

#include <fstream>
#include <iomanip>

#include "G4AntiLambdabZero.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      AntiLambdabZero                        ###
// ######################################################################

G4AntiLambdabZero * G4AntiLambdabZero::theInstance = 0 ;

G4AntiLambdabZero * G4AntiLambdabZero::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "anti_lambda_b0";
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
      new G4ParticleDefinition( name , 5.624*GeV , 5e-10*MeV , 0. ,
                                1,              +1,             0,
                                0,              0,             0,
                                "baryon", 0,  1, -5122,
                                false,     1.2e-3*ns,          NULL,
                                false, "lambda_b" );
  }
  theInstance = reinterpret_cast<G4AntiLambdabZero*>(anInstance);
  return theInstance;
}

G4AntiLambdabZero * G4AntiLambdabZero::AntiLambdabZeroDefinition() {
  return Definition( ) ;
}

G4AntiLambdabZero * G4AntiLambdabZero::AntiLambdabZero() {
  return Definition( ) ;
}
