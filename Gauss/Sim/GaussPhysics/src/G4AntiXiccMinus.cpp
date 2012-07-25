// $Id: G4AntiXiccMinus.cpp,v 1.1 2011-10-25 14:04:24 Zhenwei Yang Exp $

#include "G4AntiXiccMinus.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      AntiXiccMinus                        ###
// ######################################################################

G4AntiXiccMinus * G4AntiXiccMinus::theInstance = 0 ;

G4AntiXiccMinus * G4AntiXiccMinus::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "anti-xi_cc-";
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
      new G4ParticleDefinition( name , 3.59798*GeV , 5.e-10*MeV ,  -1.*eplus ,
                                1,              +1,             0,
                                0,              0,             0,
                                "baryon", 0,  1, -4412,
                                false,     0.334e-3*ns,          NULL,
                                false, "xi_cc" );
  }
  theInstance = reinterpret_cast<G4AntiXiccMinus*>(anInstance);
  return theInstance;
}

G4AntiXiccMinus * G4AntiXiccMinus::AntiXiccMinusDefinition() {
  return Definition( ) ;
}

G4AntiXiccMinus * G4AntiXiccMinus::AntiXiccMinus() {
  return Definition( ) ;
}
