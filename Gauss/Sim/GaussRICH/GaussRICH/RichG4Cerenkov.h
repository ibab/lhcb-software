//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: RichG4Cerenkov.hh,v 1.7 2008-08-08 09:23:14 seaso Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
////////////////////////////////////////////////////////////////////////
// Cerenkov Radiation Class Definition 
////////////////////////////////////////////////////////////////////////
//
// File:        RichG4Cerenkov.hh  
// Description:	Discrete Process - Generation of Cerenkov Photons
// Version:     2.0
// Created:     1996-02-21
// Author:      Juliet Armstrong
// Updated:     2007-09-30 change inheritance to G4VDiscreteProcess
//              2005-07-28 add G4ProcessType to constructor
//              1999-10-29 add method and class descriptors
//              1997-04-09 by Peter Gumplinger
//              > G4MaterialPropertiesTable; new physics/tracking scheme
// mail:        gum@triumf.ca
// Modifed for LHCb and renamed RichG4Cerenkov   SE 1-4-2005.
// Modified to be similar to the G4.9.1 version in 5-8-2008 SE 
//
////////////////////////////////////////////////////////////////////////

#ifndef RichG4Cerenkov_h
#define RichG4Cerenkov_h 1

/////////////
// Includes
/////////////

#include "globals.hh"
#include "templates.hh"
#include "Randomize.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleMomentum.hh"
#include "G4Step.hh"
#include "G4VDiscreteProcess.hh"
#include "G4OpticalPhoton.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh" 
#include "G4PhysicsTable.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4PhysicsOrderedFreeVector.hh"
#include <vector>

// Class Description:
// Discrete Process -- Generation of Cerenkov Photons.
// Class inherits publicly from G4VDiscreteProcess.
// Class Description - End:

/////////////////////
// Class Definition
/////////////////////

class RichG4Cerenkov : public G4VDiscreteProcess  
{

private:

        //////////////
        // Operators
        //////////////

	// RichG4Cerenkov& operator=(const RichG4Cerenkov &right);

public: // Without description

	////////////////////////////////
	// Constructors and Destructor
	////////////////////////////////

	RichG4Cerenkov(const G4String& processName = "Cerenkov", 
                            G4ProcessType type = fElectromagnetic);

	// RichG4Cerenkov(const RichG4Cerenkov &right);

	~RichG4Cerenkov();	

        ////////////
        // Methods
        ////////////

public: // With description

        G4bool IsApplicable(const G4ParticleDefinition& aParticleType);
        // Returns true -> 'is applicable', for all charged particles.

        G4double GetMeanFreePath(const G4Track& aTrack,
                                 G4double ,
                                 G4ForceCondition* );
        // Returns the discrete step limit and sets the 'StronglyForced'
        // condition for the DoIt to be invoked at every step.

	G4VParticleChange* PostStepDoIt(const G4Track& aTrack, 
					const G4Step&  aStep);
        // This is the method implementing the Cerenkov process.

	void SetTrackSecondariesFirst(const G4bool state);
        // If set, the primary particle tracking is interrupted and any 
        // produced Cerenkov photons are tracked next. When all have 
        // been tracked, the tracking of the primary resumes. 
	
	void SetMaxNumPhotonsPerStep(const G4int NumPhotons);
        // Set the maximum number of Cerenkov photons allowed to be 
        // generated during a tracking step. This is an average ONLY; 
        // the actual number will vary around this average. If invoked, 
        // the maximum photon stack will roughly be of the size set.
        // If not called, the step is not limited by the number of 
        // photons generated.

        G4PhysicsTable* GetPhysicsTable() const;
        // Returns the address of the physics table.

        void DumpPhysicsTable() const;
        // Prints the physics table.

       G4bool GetRichVerboseInfoTag(){return  fRichVerboseInfoTag;}
       void SetRichVerboseInfoTag(G4bool aVTagValue)
             {fRichVerboseInfoTag = aVTagValue;}
        
       void SetMaxPhotonPerRadiatorFlag
       (const G4bool aMaxNumPerStepRadiatorFlag)
       {fMaxPhotonPerRadiatorFlag=aMaxNumPerStepRadiatorFlag;}

private:

        void BuildThePhysicsTable();

	/////////////////////
	// Helper Functions
	/////////////////////

	G4double GetAverageNumberOfPhotons(const G4double charge,
                                const G4double beta,
		    		const G4Material *aMaterial,
				    G4MaterialPropertyVector* Rindex) const;

        ///////////////////////
        // Class Data Members
        ///////////////////////

protected:

        G4PhysicsTable* thePhysicsTable;
        //  A Physics Table can be either a cross-sections table or
        //  an energy table (or can be used for other specific
        //  purposes).

private:

	G4bool fTrackSecondariesFirst;
	G4int  fMaxPhotons;
  G4bool fRichVerboseInfoTag;
  G4bool fMaxPhotonPerRadiatorFlag;
};

////////////////////
// Inline methods
////////////////////

inline 
G4bool RichG4Cerenkov::IsApplicable(const G4ParticleDefinition& aParticleType)
{
   if (aParticleType.GetParticleName() != "chargedgeantino" ) {
      return (aParticleType.GetPDGCharge() != 0);
   } else {
      return false;
   }
}

inline 
void RichG4Cerenkov::SetTrackSecondariesFirst(const G4bool state) 
{ 
	fTrackSecondariesFirst = state;
}

inline
void RichG4Cerenkov::SetMaxNumPhotonsPerStep(const G4int NumPhotons) 
{ 
	fMaxPhotons = NumPhotons;
}

inline
void RichG4Cerenkov::DumpPhysicsTable() const
{
        G4int PhysicsTableSize = thePhysicsTable->entries();
        G4PhysicsOrderedFreeVector *v;

        for (G4int i = 0 ; i < PhysicsTableSize ; i++ )
        {
        	v = (G4PhysicsOrderedFreeVector*)(*thePhysicsTable)[i];
        	v->DumpValues();
        }
}

inline G4PhysicsTable* RichG4Cerenkov::GetPhysicsTable() const
{
  return thePhysicsTable;
}

#endif /* RichG4Cerenkov_h */
