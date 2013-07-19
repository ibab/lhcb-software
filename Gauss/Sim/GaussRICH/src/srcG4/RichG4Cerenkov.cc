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
// $Id: RichG4Cerenkov.cc,v 1.15 2009-09-13 13:07:18 seaso Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
////////////////////////////////////////////////////////////////////////
// Cerenkov Radiation Class Implementation
////////////////////////////////////////////////////////////////////////
//
// File:        RichG4Cerenkov.cc 
// Description: Discrete Process -- Generation of Cerenkov Photons
// Version:     2.1
// Created:     1996-02-21  
// Author:      Juliet Armstrong
// Updated:     2007-09-30 by Peter Gumplinger
//              > change inheritance to G4VDiscreteProcess
//              GetContinuousStepLimit -> GetMeanFreePath (StronglyForced)
//              AlongStepDoIt -> PostStepDoIt
//              2005-08-17 by Peter Gumplinger
//              > change variable name MeanNumPhotons -> MeanNumberOfPhotons
//              2005-07-28 by Peter Gumplinger
//              > add G4ProcessType to constructor
//              2001-09-17, migration of Materials to pure STL (mma) 
//              2000-11-12 by Peter Gumplinger
//              > add check on CerenkovAngleIntegrals->IsFilledVectorExist()
//              in method GetAverageNumberOfPhotons 
//              > and a test for MeanNumberOfPhotons <= 0.0 in DoIt
//              2000-09-18 by Peter Gumplinger
//              > change: aSecondaryPosition=x0+rand*aStep.GetDeltaPosition();
//                        aSecondaryTrack->SetTouchable(0);
//              1999-10-29 by Peter Gumplinger
//              > change: == into <= in GetContinuousStepLimit
//              1997-08-08 by Peter Gumplinger
//              > add protection against /0
//              > G4MaterialPropertiesTable; new physics/tracking scheme
//
// mail:        gum@triumf.ca
//
//  modifed for LHCb and renamed RichG4Cerenkov SE 1-4-2005
//  modified to be similar to that in  G4.9.1 on 5-8-2008 SE
////////////////////////////////////////////////////////////////////////

#include "G4ios.hh"
#include "G4Poisson.hh"
#include "GaussRICH/RichG4Cerenkov.h"
// To tag the info regarding photon production.
#include "GaussRICH/RichG4CherenkovPhotProdTag.h"
#include "GaussRICH/RichG4MatRadIdentifier.h"
// local analysis for RichG4.
//#include "RichG4CherenkovAnalysis.h"
#include "GaussRICH/RichG4QwAnalysis.h"

// end of include localanalysis

using namespace std;

/////////////////////////
// Class Implementation  
/////////////////////////

        //////////////
        // Operators
        //////////////

// RichG4Cerenkov::operator=(const RichG4Cerenkov &right)
// {
// }

        /////////////////
        // Constructors
        /////////////////

RichG4Cerenkov::RichG4Cerenkov(const G4String& processName, G4ProcessType type)
  : G4VDiscreteProcess(processName, type),
    fRichVerboseInfoTag(false),
    fMaxPhotonPerRadiatorFlag(false)
{

  //        G4cout << "RichG4Cerenkov::RichG4Cerenkov constructor" << G4endl;
  //      G4cout << "NOTE: this is now a G4VDiscreteProcess!" << G4endl;
  //      G4cout << "Required change in UserPhysicsList: " << G4endl;
  //       G4cout << "change: pmanager->AddContinuousProcess(theCerenkovProcess);" << G4endl;
  //      G4cout << "to:     pmanager->AddProcess(theCerenkovProcess);" << G4endl;
  //      G4cout << "        pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);" << G4endl;

	fTrackSecondariesFirst = false;
	fMaxPhotons = 0;

        thePhysicsTable = NULL;

	if (verboseLevel>0) {
           G4cout << GetProcessName() << " is created " << G4endl;
	}

	BuildThePhysicsTable();
}


// RichG4Cerenkov::RichG4Cerenkov(const RichG4Cerenkov &right)
// {
// }

        ////////////////
        // Destructors
        ////////////////

RichG4Cerenkov::~RichG4Cerenkov() 
{
	if (thePhysicsTable != NULL) {
	   thePhysicsTable->clearAndDestroy();
           delete thePhysicsTable;
	}
}

        ////////////
        // Methods
        ////////////

// PostStepDoIt
// -------------
//
G4VParticleChange*
RichG4Cerenkov::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)

// This routine is called for each tracking Step of a charged particle
// in a radiator. A Poisson-distributed number of photons is generated
// according to the Cerenkov formula, distributed evenly along the track
// segment and uniformly azimuth w.r.t. the particle direction. The 
// parameters are then transformed into the Master Reference System, and 
// they are added to the particle change. 

{

	//////////////////////////////////////////////////////
	// Should we ensure that the material is dispersive?
	//////////////////////////////////////////////////////

        aParticleChange.Initialize(aTrack);

        const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
        const G4Material* aMaterial = aTrack.GetMaterial();
        G4MaterialPropertiesTable* aMaterialPropertiesTable =
                               aMaterial->GetMaterialPropertiesTable();
        if (!aMaterialPropertiesTable)
           return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

	G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
	G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
  // test printout by SE

  // G4String  pPreVolName=  pPreStepPoint->
  //              GetPhysicalVolume()->GetLogicalVolume()->GetName();
  // G4String  pPostVolName=  pPostStepPoint->
  //             GetPhysicalVolume()->GetLogicalVolume()->GetName();
  //      G4String aMaterialName= aMaterial->GetName();
  // G4cout<<"G4Cerenkov: PreVol PostVol Material Names: "<<pPreVolName<<"    "
  //       << pPostVolName<<"   "<< aMaterialName<< G4endl;


  // end of test printout by SE

	G4ThreeVector x0 = pPreStepPoint->GetPosition();
        G4ThreeVector p0 = aStep.GetDeltaPosition().unit();
	G4double t0 = pPreStepPoint->GetGlobalTime();


       //Change by SE to produce photons only within a limited range of RefIndex.
        //	const G4MaterialPropertyVector* Rindex = 
        //        aMaterialPropertiesTable->GetProperty("RINDEX"); 
      	 G4MaterialPropertyVector* Rindex = 
                aMaterialPropertiesTable->GetProperty("CKVRNDX"); 

        if (!Rindex)return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

        // particle charge
        const G4double charge = aParticle->GetDefinition()->GetPDGCharge();

        // particle beta
        const G4double beta = (pPreStepPoint ->GetBeta() +
                               pPostStepPoint->GetBeta())/2.;

	G4double MeanNumberOfPhotons = 
                 GetAverageNumberOfPhotons(charge,beta,aMaterial,Rindex);

        if (MeanNumberOfPhotons <= 0.0) {

                // return unchanged particle and no secondaries

                aParticleChange.SetNumberOfSecondaries(0);
 
                return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

        }

        G4double step_length;
        step_length = aStep.GetStepLength();

	MeanNumberOfPhotons = MeanNumberOfPhotons * step_length;

	G4int NumPhotons = (G4int) G4Poisson(MeanNumberOfPhotons);

	if (NumPhotons <= 0) {

		// return unchanged particle and no secondaries  

		aParticleChange.SetNumberOfSecondaries(0);
		
                return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
	}

	////////////////////////////////////////////////////////////////

	aParticleChange.SetNumberOfSecondaries(NumPhotons);

        if (fTrackSecondariesFirst) {
           if (aTrack.GetTrackStatus() == fAlive )
                   aParticleChange.ProposeTrackStatus(fSuspend);
        }
	
	////////////////////////////////////////////////////////////////

  // following two lines  modified in Sept 10-2009 using G4.9.2 
	// G4double Pmin = Rindex->GetMinPhotonMomentum(); // for versions until G4.9.1
	//G4double Pmax = Rindex->GetMaxPhotonMomentum();  // for versions until G4.9.1

        //  G4double Pmin = Rindex->GetMinPhotonEnergy(); //for the  G4 versions after 4.9.1
        //	G4double Pmax = Rindex->GetMaxPhotonEnergy();   //for the  G4 versions after 4.9.1 

        G4double Pmin = Rindex->GetMinLowEdgeEnergy(); // for G4.95 onwards
        G4double Pmax = Rindex->GetMaxLowEdgeEnergy();  //for G4.95.onwards


	G4double dp = Pmax - Pmin;

  //	G4double nMax = Rindex->GetMaxProperty(); // for G4.95 onwards
	G4double nMax = Rindex->GetMaxValue();

        G4double BetaInverse = 1./beta;

	G4double maxCos = BetaInverse / nMax; 
	G4double maxSin2 = (1.0 - maxCos) * (1.0 + maxCos);

	for (G4int i = 0; i < NumPhotons; i++) {

		// Determine photon energy

		G4double rand;
		G4double sampledEnergy, sampledRI; 
		G4double cosTheta, sin2Theta;
		
		// sample an energy

		do {
			rand = G4UniformRand();	
			sampledEnergy = Pmin + rand * dp; 
			sampledRI = Rindex->Value(sampledEnergy);
			cosTheta = BetaInverse / sampledRI;  

			sin2Theta = (1.0 - cosTheta)*(1.0 + cosTheta);
			rand = G4UniformRand();	

		} while (rand*maxSin2 > sin2Theta);

		// Generate random position of photon on cone surface 
		// defined by Theta 

		rand = G4UniformRand();

		G4double phi = twopi*rand;
		G4double sinPhi = sin(phi);
		G4double cosPhi = cos(phi);

		// calculate x,y, and z components of photon energy
		// (in coord system with primary particle direction 
		//  aligned with the z axis)

		G4double sinTheta = sqrt(sin2Theta); 
		G4double px = sinTheta*cosPhi;
		G4double py = sinTheta*sinPhi;
		G4double pz = cosTheta;

		// Create photon momentum direction vector 
		// The momentum direction is still with respect
	 	// to the coordinate system where the primary
		// particle direction is aligned with the z axis  

		G4ParticleMomentum photonMomentum(px, py, pz);

		// Rotate momentum direction back to global reference
		// system 

                photonMomentum.rotateUz(p0);

		// Determine polarization of new photon 

		G4double sx = cosTheta*cosPhi;
		G4double sy = cosTheta*sinPhi; 
		G4double sz = -sinTheta;

		G4ThreeVector photonPolarization(sx, sy, sz);

		// Rotate back to original coord system 

                photonPolarization.rotateUz(p0);
		
                // Generate a new photon:

                G4DynamicParticle* aCerenkovPhoton =
                  new G4DynamicParticle(G4OpticalPhoton::OpticalPhoton(), 
  					                 photonMomentum);
		aCerenkovPhoton->SetPolarization
				     (photonPolarization.x(),
				      photonPolarization.y(),
				      photonPolarization.z());

		aCerenkovPhoton->SetKineticEnergy(sampledEnergy);

                // Generate new G4Track object:

		rand = G4UniformRand();

    G4double delta = rand * aStep.GetStepLength();
		G4double deltaTime = delta /
                       ((pPreStepPoint->GetVelocity()+
                         pPostStepPoint->GetVelocity())/2.);
    G4double aSecondaryTime = t0 + deltaTime;

    G4ThreeVector aSecondaryPosition =
                  x0 + rand * aStep.GetDeltaPosition();

		G4Track* aSecondaryTrack = 
		new G4Track(aCerenkovPhoton,aSecondaryTime,aSecondaryPosition);

    aSecondaryTrack->SetTouchableHandle((G4VTouchable*)0);

    aSecondaryTrack->SetParentID(aTrack.GetTrackID());

    //Additions made by SE to Tag the info regarding photon
    //production. SE 15-10-2002.

    
    G4Track* aTaggedSecondaryTrack
      = RichG4CherenkovPhotProdTag(aTrack,aSecondaryTrack,
                                   cosTheta,phi,sampledEnergy,fRichVerboseInfoTag,1 );
    aParticleChange.AddSecondary(aTaggedSecondaryTrack);
    // end of additions by SE to tag the info on photon production.
    // also the next line is commented out as as result.
    // aParticleChange.AddSecondary(aSecondaryTrack);
		//aParticleChange.AddSecondary(aSecondaryTrack);

	}
   // test analysis by SE. Not needed for regular production running.
  // RichG4CherenkovProdFeaturesHisto(aTrack);
  // RichG4QwAnalysis* aRichG4QwAnalysis = RichG4QwAnalysis::getRichG4QwAnalysisInstance();
  // if( aRichG4QwAnalysis->qwAnalysisNtupleBooked() ) {
  //  aRichG4QwAnalysis ->FillQwAnalysisHisto(aTrack);
  // }
  // end of test analysis by SE

	if (verboseLevel>0) {
	G4cout << "\n Exiting from RichG4Cerenkov::DoIt -- NumberOfSecondaries = " 
	     << aParticleChange.GetNumberOfSecondaries() << G4endl;
	}

	return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
}


// BuildThePhysicsTable for the Cerenkov process
// ---------------------------------------------
//


void RichG4Cerenkov::BuildThePhysicsTable()
{
	if (thePhysicsTable) return;

	const G4MaterialTable* theMaterialTable=
	 		       G4Material::GetMaterialTable();
	G4int numOfMaterials = G4Material::GetNumberOfMaterials();

	// create new physics table
	
	thePhysicsTable = new G4PhysicsTable(numOfMaterials);

	// loop for materials

	for (G4int i=0 ; i < numOfMaterials; i++)
	{
		G4PhysicsOrderedFreeVector* aPhysicsOrderedFreeVector =
					new G4PhysicsOrderedFreeVector();

		// Retrieve vector of refraction indices for the material
		// from the material's optical properties table 

		G4Material* aMaterial = (*theMaterialTable)[i];

		G4MaterialPropertiesTable* aMaterialPropertiesTable =
				aMaterial->GetMaterialPropertiesTable();

		if (aMaterialPropertiesTable) {




      //Change by SE to produce photons only within a limited range of RefIndex.
		  // G4MaterialPropertyVector* theRefractionIndexVector = 
		  //  	   aMaterialPropertiesTable->GetProperty("RINDEX");

		   G4MaterialPropertyVector* theRefractionIndexVector = 
		    	   aMaterialPropertiesTable->GetProperty("CKVRNDX");
      //end of change by SE

		   if (theRefractionIndexVector) {
		
		      // Retrieve the first refraction index in vector
		      // of (photon energy, refraction index) pairs 

                      G4double currentRI = (*theRefractionIndexVector)[0];

		      if (currentRI > 1.0) {

			 // Create first (photon energy, Cerenkov Integral)
			 // pair  

                         G4double currentPM = theRefractionIndexVector->
                                                 Energy(0);
			 G4double currentCAI = 0.0;

			 aPhysicsOrderedFreeVector->
			 	 InsertValues(currentPM , currentCAI);

			 // Set previous values to current ones prior to loop

			 G4double prevPM  = currentPM;
			 G4double prevCAI = currentCAI;
                	 G4double prevRI  = currentRI;

			 // loop over all (photon energy, refraction index)
			 // pairs stored for this material  

                         for (size_t i = 1;
                              i < theRefractionIndexVector->GetVectorLength();
                              i++)
			 {
                                currentRI = (*theRefractionIndexVector)[i];
                                currentPM = theRefractionIndexVector->Energy(i);

				currentCAI = 0.5*(1.0/(prevRI*prevRI) +
					          1.0/(currentRI*currentRI));

				currentCAI = prevCAI + 
					     (currentPM - prevPM) * currentCAI;

				aPhysicsOrderedFreeVector->
				    InsertValues(currentPM, currentCAI);

				prevPM  = currentPM;
				prevCAI = currentCAI;
				prevRI  = currentRI;
			 }

		      }
		   }
		}

	// The Cerenkov integral for a given material
	// will be inserted in thePhysicsTable
	// according to the position of the material in
	// the material table. 

	thePhysicsTable->insertAt(i,aPhysicsOrderedFreeVector); 

	}
}

// GetMeanFreePath
// ---------------
//

G4double RichG4Cerenkov::GetMeanFreePath(const G4Track& aTrack,
                                           G4double,
                                           G4ForceCondition* condition)
{
        *condition = StronglyForced;

	// If user has defined an average maximum number of photons to
	// be generated in a Step, then return the Step length for that 
	// number of photons. 
 
	if (fMaxPhotons <= 0) return DBL_MAX;

        const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
        const G4Material* aMaterial = aTrack.GetMaterial();

        G4MaterialPropertiesTable* aMaterialPropertiesTable =
                               aMaterial->GetMaterialPropertiesTable();
        if (!aMaterialPropertiesTable) return DBL_MAX;

       //Change by SE to produce photons only within a limited range of RefIndex.
       //      const G4MaterialPropertyVector* Rindex =
       //              aMaterialPropertiesTable->GetProperty("RINDEX");

         G4MaterialPropertyVector* Rindex =
                aMaterialPropertiesTable->GetProperty("CKVRNDX");
       //end of change by SE

        if (!Rindex) return DBL_MAX;

        // particle charge
        const G4double charge = aParticle->GetDefinition()->GetPDGCharge();

        // particle beta
        const G4double beta = aParticle->GetTotalMomentum() /
                              aParticle->GetTotalEnergy();

	G4double MeanNumberOfPhotons = 
                 GetAverageNumberOfPhotons(charge,beta,aMaterial,Rindex);

        if(MeanNumberOfPhotons <= 0.0) return DBL_MAX;

	G4double StepLimit = fMaxPhotons / MeanNumberOfPhotons;

  // Change by SE to modify the step limit for different
  // radiators if the flag for that is switched on.
  // June 2004.
  // modified in Feb 2006 to account for the existance of several radiators.

  if( fMaxPhotonPerRadiatorFlag) {
    RichG4MatRadIdentifier* aRichG4MatRadIdentifier =
      RichG4MatRadIdentifier::RichG4MatRadIdentifierInstance();
    G4int CurMaxPhotPerStepLim =
      aRichG4MatRadIdentifier-> getRadiatorPhotPerStepLimit(aMaterial-> GetIndex());
    StepLimit = CurMaxPhotPerStepLim/MeanNumberOfPhotons;
  }
  
  // end of modification by SE

	return StepLimit;
}

// GetAverageNumberOfPhotons
// -------------------------
// This routine computes the number of Cerenkov photons produced per
// GEANT-unit (millimeter) in the current medium. 
//             ^^^^^^^^^^


G4double RichG4Cerenkov::GetAverageNumberOfPhotons(const G4double charge,
                              const G4double beta, 
			      const G4Material* aMaterial,
			      G4MaterialPropertyVector* Rindex) const
{
	const G4double Rfact = 369.81/(eV * cm);

        if(beta <= 0.0)return 0.0;

        G4double BetaInverse = 1./beta;

	// Vectors used in computation of Cerenkov Angle Integral:
	// 	- Refraction Indices for the current material
	//	- new G4PhysicsOrderedFreeVector allocated to hold CAI's
 
	G4int materialIndex = aMaterial->GetIndex();

	// Retrieve the Cerenkov Angle Integrals for this material  

	G4PhysicsOrderedFreeVector* CerenkovAngleIntegrals =
	(G4PhysicsOrderedFreeVector*)((*thePhysicsTable)(materialIndex));

        if(!(CerenkovAngleIntegrals->IsFilledVectorExist()))return 0.0;

	// Min and Max photon energies
  G4double Pmin = Rindex->GetMinLowEdgeEnergy(); //G4.95 onwards
  G4double Pmax = Rindex->GetMaxLowEdgeEnergy(); //G4.95 onwards

	// Min and Max Refraction Indices 
	G4double nMin = Rindex->GetMinValue();	
	G4double nMax = Rindex->GetMaxValue();

	// Max Cerenkov Angle Integral 
	G4double CAImax = CerenkovAngleIntegrals->GetMaxValue();

	G4double dp, ge;

	// If n(Pmax) < 1/Beta -- no photons generated 

	if (nMax < BetaInverse) {
		dp = 0;
		ge = 0;
	} 

	// otherwise if n(Pmin) >= 1/Beta -- photons generated  

	else if (nMin > BetaInverse) {
		dp = Pmax - Pmin;	
		ge = CAImax; 
	} 

	// If n(Pmin) < 1/Beta, and n(Pmax) >= 1/Beta, then
	// we need to find a P such that the value of n(P) == 1/Beta.
	// Interpolation is performed by the GetEnergy() and
	// Value() methods of the G4MaterialPropertiesTable and
	// the GetValue() method of G4PhysicsVector.  

	else {
		Pmin = Rindex->GetEnergy(BetaInverse);
		dp = Pmax - Pmin;

		// need boolean for current implementation of G4PhysicsVector
		// ==> being phased out
		G4bool isOutRange;
		G4double CAImin = CerenkovAngleIntegrals->
                                  GetValue(Pmin, isOutRange);
		ge = CAImax - CAImin;

		if (verboseLevel>0) {
			G4cout << "CAImin = " << CAImin << G4endl;
			G4cout << "ge = " << ge << G4endl;
		}
	}
	
	// Calculate number of photons 
	G4double NumPhotons = Rfact * charge/eplus * charge/eplus *
                                 (dp - ge * BetaInverse*BetaInverse);

	return NumPhotons;		
}



