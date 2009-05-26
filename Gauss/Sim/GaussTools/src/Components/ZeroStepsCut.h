// $Id: ZeroStepsCut.h,v 1.1 2009-05-26 17:14:25 gcorti Exp $
#ifndef GAUSSTOOLS_COMPONENTS_ZEROSTEPSGCUT_H 
#define GAUSSTOOLS_COMPONENTS_ZEROSTEPSCUT_H 1

// Include files
#include "G4ios.hh"
#include "globals.hh"
#include "SpecialCuts.h"

/** @class ZeroStepsCut ZeroStepsCut.h Components/ZeroStepsCut.h
 *  
 *  Kill particles that are stuck in a volume, i.e. make very very small
 *  steps in the same volume (universe is allowed).
 *  The check is done after a certain number of steps. Both the number of
 *  steps after which the check is done, the step lenght below which the
 *  step is considered zero and the number of zero steps are set at 
 *  configuration 
 *
 *  @author Gloria CORTI
 *  @date   2009-03-26
 */
  
namespace GiGa {
  
class ZeroStepsCut : public SpecialCuts {
public: 
  /// Constructor
  /// @param nMaxStep    Number of step after which the check is applied
  /// @param maxLenght   Maximum step lenght, below which the check is applied
  /// @param nMaxRepeat  Maximum number of pingpong repetions allowed
  ZeroStepsCut( const G4String& processName ="ZeroStepsCut",
                int nTotSteps = 1000000, 
                double maxLenght = 1.0e-9,  
                int nMaxZeroSteps = 20.,
                const G4String& world = "Universe" ); 
  
  virtual ~ZeroStepsCut( ); ///< Destructor
  
  /// Implementation of G4VProcess method
  virtual G4double PostStepGetPhysicalInteractionLength(
                                          const G4Track& track,
                                          G4double   previousStepSize,
                                          G4ForceCondition* condition );
            
  /// Implementation of G4VProcess method
  virtual G4VParticleChange* PostStepDoIt( const G4Track&, const G4Step& );

protected:

private:

  /// Assignment operator as private 
  ZeroStepsCut(ZeroStepsCut&);
  ZeroStepsCut& operator=(const ZeroStepsCut& right);

  /// Counter for number of pingpong between volumes
  int m_countZero;
  /// Max number of steps on a track before checking for pingpong
  int m_maxNumSteps;
  /// Max number of repeated steps to define pingpong
  int m_maxZeroSteps;
  /// Initial max step lenght below which step is considered for pingpong
  double m_maxStepLenght;
    
  /// Pointer to Geant4 Physical Volume of previous step
  G4VPhysicalVolume* m_prevVol;          
  G4String m_worldVol;
    
};
 
} // closes namespace

#endif // GAUSSTOOLS_COMPONENTS_ZEROSTEPSCUT_H
