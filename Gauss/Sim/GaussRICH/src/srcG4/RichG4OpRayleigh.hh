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
// $Id: RichG4OpRayleigh.hh,v 1.7 2008-01-21 16:53:44 seaso Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
////////////////////////////////////////////////////////////////////////
// Optical Photon Rayleigh Scattering Class Definition
////////////////////////////////////////////////////////////////////////
//
// File:        RichG4OpRayleigh.hh
// Description: Discrete Process -- Rayleigh scattering of optical photons 
// Version:     1.0
// Created:     1996-05-31
// Author:      Juliet Armstrong
// Updated:     1999-10-29 add method and class descriptors
//              1997-04-09 by Peter Gumplinger
//              > new physics/tracking scheme
// mail:        gum@triumf.ca
// modifed for LHCb and renamed to RichG4OpRayleigh  SE 1-4-2005
////////////////////////////////////////////////////////////////////////

#ifndef RichG4OpRayleigh_h
#define RichG4OpRayleigh_h 1

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
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4OpticalPhoton.hh"
#include "G4PhysicsTable.hh"
#include "G4PhysicsOrderedFreeVector.hh"

// Class Description:
// Discrete Process -- Rayleigh scattering of optical photons.
// Class inherits publicly from G4VDiscreteProcess.
// Class Description - End:

/////////////////////
// Class Definition
/////////////////////

class RichG4OpRayleigh : public G4VDiscreteProcess 
{

private:
 
        //////////////
        // Operators
        //////////////

        // RichG4OpRayleigh& operator=(const RichG4OpRayleigh &right);

public: // Without description

        ////////////////////////////////
        // Constructors and Destructor
        ////////////////////////////////
 
        RichG4OpRayleigh(const G4String& processName = "RichG4OpRayleigh",
                    G4ProcessType aType =   fOptical);

        // RichG4OpRayleigh(const RichG4OpRayleigh &right);

	~RichG4OpRayleigh();

        ////////////
        // Methods
        ////////////

public: // With description

        G4bool IsApplicable(const G4ParticleDefinition& aParticleType);
        // Returns true -> 'is applicable' only for an optical photon.

        G4double GetMeanFreePath(const G4Track& aTrack,
				 G4double ,
                                 G4ForceCondition* );
        // Returns the mean free path for Rayleigh scattering in water.
        // --- Not yet implemented for other materials! ---

        G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                       const G4Step&  aStep);
        // This is the method implementing Rayleigh scattering.

        G4PhysicsTable* GetPhysicsTable() const;
        // Returns the address of the physics table.

        void DumpPhysicsTable() const;
        // Prints the physics table.
       G4bool GetRichVerboseRayleighInfoTag(){ return fRichVerboseInfoTag;}
       void SetRichVerboseRayleighInfoTag( G4bool aBVal)
             {fRichVerboseInfoTag = aBVal;}
  
     //  The above are for verbose information tagging for Output. 

        G4int GetRichMaxStepNumLimitInRayleigh() 
       {    return fRichMaxStepNumLimitInRayleigh;}
       void SetRichMaxStepNumLimitInRayleigh(G4int aStepNumLimit) 
       {fRichMaxStepNumLimitInRayleigh = aStepNumLimit;}
       G4int GetMaxNumRayleighScatAllowed() 
       {  return fMaxNumRayleighScatAllowed;}
       void SetMaxNumRayleighScatAllowed(G4int aMaxNumRly) 
       {   fMaxNumRayleighScatAllowed= aMaxNumRly;}
  

private:

        void BuildThePhysicsTable();

        /////////////////////
        // Helper Functions
        /////////////////////

	G4PhysicsOrderedFreeVector* RayleighAttenuationLengthGenerator(
					G4MaterialPropertiesTable *aMPT);

        ///////////////////////
        // Class Data Members
        ///////////////////////
      G4bool fRichVerboseInfoTag;
      G4int fRichMaxStepNumLimitInRayleigh;
      G4int fMaxNumRayleighScatAllowed;
  

protected:

        G4PhysicsTable* thePhysicsTable;
        //  A Physics Table can be either a cross-sections table or
        //  an energy table (or can be used for other specific
        //  purposes).

private:

        G4bool DefaultWater;

};

////////////////////
// Inline methods
////////////////////

inline
G4bool RichG4OpRayleigh::IsApplicable(const G4ParticleDefinition& aParticleType)
{
  return ( &aParticleType == G4OpticalPhoton::OpticalPhoton() );
}

inline
void RichG4OpRayleigh::DumpPhysicsTable() const

{
        G4int PhysicsTableSize = thePhysicsTable->entries();
        G4PhysicsOrderedFreeVector *v;

        for (G4int i = 0 ; i < PhysicsTableSize ; i++ )
        {
                v = (G4PhysicsOrderedFreeVector*)(*thePhysicsTable)[i];
                v->DumpValues();
        }
}

inline G4PhysicsTable* RichG4OpRayleigh::GetPhysicsTable() const
{
  return thePhysicsTable;
}


#endif /* RichG4OpRayleigh_h */
