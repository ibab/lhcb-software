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
// $Id: RichPhotoElectron.cpp,v 1.5 2006-01-10 14:58:23 seaso Exp $
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
//
//#include "g4std/fstream"
//#include "g4std/iomanip"
    
// #include "G4Electron.hh"
#include "RichPhotoElectron.h"
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
  }
  theInstance = reinterpret_cast<RichPhotoElectron*>(anInstance);
  return theInstance;
}

RichPhotoElectron* RichPhotoElectron::PhotoElectronDefinition()
                       {return Definition();}

RichPhotoElectron* RichPhotoElectron::PhotoElectron()
                      {  return Definition();}
 





