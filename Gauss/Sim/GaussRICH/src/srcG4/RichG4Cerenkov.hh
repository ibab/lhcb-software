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
// $Id: RichG4Cerenkov.hh,v 1.2 2004-02-04 13:52:24 seaso Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
////////////////////////////////////////////////////////////////////////
// Cerenkov Radiation Class Definition 
////////////////////////////////////////////////////////////////////////
//        RichG4Cerenkov.hh  SE 28-04-2003
//
// File:        G4Cerenkov.hh  
// Description:	Continuous Process -- Generation of Cerenkov Photons
// Version:     2.0
// Created:     1996-02-21
// Author:      Juliet Armstrong
// Updated:     1999-10-29 add method and class descriptors
//              1997-04-09 by Peter Gumplinger
//              > G4MaterialPropertiesTable; new physics/tracking scheme
// mail:        gum@triumf.ca
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
#include "G4VContinuousProcess.hh"
#include "G4OpticalPhoton.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh" 
#include "G4PhysicsTable.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4PhysicsOrderedFreeVector.hh"

// Class Description:
// Continuous Process -- Generation of Cerenkov Photons.
// Class inherits publicly from G4VContinuousProcess.
// Class Description - End:

/////////////////////
// Class Definition
/////////////////////

class RichG4Cerenkov : public G4VContinuousProcess  
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

	RichG4Cerenkov(const G4String& processName = "RichG4Cerenkov");

	// RichG4Cerenkov(const RichG4Cerenkov &right);

	~RichG4Cerenkov();	

        ////////////
        // Methods
        ////////////

public: // With description

        G4bool IsApplicable(const G4ParticleDefinition& aParticleType);
        // Returns true -> 'is applicable', for all charged particles.

	G4double GetContinuousStepLimit(const G4Track& aTrack,
					G4double  ,
					G4double  ,
                                        G4double& );
        // Returns the continuous step limit defined by the Cerenkov
        // process.

	G4VParticleChange* AlongStepDoIt(const G4Track& aTrack, 
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
  
    
    
private:

        void BuildThePhysicsTable();

	/////////////////////
	// Helper Functions
	/////////////////////

	G4double GetAverageNumberOfPhotons(const G4DynamicParticle *aParticle,
		    		const G4Material *aMaterial,
				const G4MaterialPropertyVector* Rindex) const;

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
  
};

////////////////////
// Inline methods
////////////////////

inline 
G4bool RichG4Cerenkov::IsApplicable(const G4ParticleDefinition& aParticleType)
{
   return (aParticleType.GetPDGCharge() != 0);
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
