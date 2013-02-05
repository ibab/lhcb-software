//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: RichPhotoElectron.cpp,v 1.7 2008-01-25 16:01:53 seaso Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// 
// 
// ----------------------------------------------------------------------
//      GEANT 4 class implementation file
//
//      History: first implementation, based on object model of
//      4th April 1996, G.Cosmo
// **********************************************************************
//  Added particle definitions, H.Kurashige, 19 April 1996
//  Added SetCuts implementation, L.Urban, 30 May 1996
//  Revised, G.Cosmo, 6 June 1996
//  Code uses operators (+=, *=, ++, -> etc.) correctly, P. Urban, 26/6/96
//  Add ElectronDefinition() H.Kurashige 4 July 1996
// ----------------------------------------------------------------------
// RichPhotoelectron created by SE 10-3-2003.
// Modifed according to G4.8   SE 9-1-2006
// Modifed according to G4.9.1   SE 25-1-2008, but commented out
//  since it is not yet compatible with the lhcb version of G4.
//
//#include "g4std/fstream"
//#include "g4std/iomanip"
    
// #include "G4Electron.hh"
#include "GaussRICH/RichPhotoElectron.h"
#include "G4ParticleTable.hh"

 

// ######################################################################
// ###                         RICHPHOTOELECTRON                      ###
// ######################################################################
RichPhotoElectron* RichPhotoElectron::theInstance=0;

RichPhotoElectron* RichPhotoElectron::Definition() 
{
  if (theInstance !=0) return theInstance;
  const G4String name = "pe-";
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
   anInstance = new G4ParticleDefinition(
                 name,  0.51099906*MeV,       0.0*MeV,    -1.*eplus,
                    1,               0,             0,
                    0,               0,             0,
             "lepton",               1,             0,          9000011,
                 true,            -1.0,          NULL,
             false,           "e"
              );
    // Bohr Magnetron
   //   G4double muB =  -0.5*eplus*hbar_Planck/(0.51099906*MeV/c_squared) ;
   //
   // anInstance->SetPDGMagneticMoment( muB * 1.0011596521859 );


  }
  theInstance = reinterpret_cast<RichPhotoElectron*>(anInstance);
  return theInstance;
}

RichPhotoElectron* RichPhotoElectron::PhotoElectronDefinition()
                       {return Definition();}

RichPhotoElectron* RichPhotoElectron::PhotoElectron()
                      {  return Definition();}
 





