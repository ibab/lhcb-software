// $Id: PingPongCut.h,v 1.1 2009-04-05 17:49:48 gcorti Exp $
#ifndef GAUSSTOOLS_COMPONENTS_PINGPONGCUT_H 
#define GAUSSTOOLS_COMPONENTS_PINGPONGCUT_H 1

// Include files
#include "G4ios.hh"
#include "globals.hh"
#include "SpecialCuts.h"

/** @class PingPongCut PingPongCut.h Components/PingPongCut.h
 *  
 *  Kill particles that ping pong between volumes, i.e. make small number 
 *  of back and forth steps of similar or identical small lenght. 
 *  The check is done after a certain number of steps. Both the number of
 *  steps after which the check is done, the step lenght below which the
 *  check is initiated and the number of repetition to define a ping pong
 *  are set at construction time.  
 *
 *  @author Gloria CORTI
 *  @date   2009-03-26
 */
  
namespace GiGa {
  
class PingPongCut : public SpecialCuts {
public: 
  /// Constructor
  /// @param nMaxStep    Number of step after which the check is applied
  /// @param maxLenght   Maximum step lenght, below which the check is applied
  /// @param nMaxRepeat  Maximum number of pingpong repetions allowed
  PingPongCut(const G4String& processName ="PingPongCut",
              int nMaxStep = 1000000, 
              double maxLenght = 1.0e-3,  
              int nMaxRepeat = 20 ); 
  
  virtual ~PingPongCut( ); ///< Destructor
  
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
  PingPongCut(PingPongCut&);
  PingPongCut& operator=(const PingPongCut& right);

  /// Counter for number of pingpong between volumes
  int m_countRepeat;
  /// Max number of steps on a track before checking for pingpong
  int m_maxNumSteps;
  /// Max number of repeated steps to define pingpong
  int m_maxRepeatSteps;
  /// Initial max step lenght below which step is considered for pingpong
  double m_maxStepLenght;
  /// Step lenght of previous step
  double m_prevStepLenght;
  /// Fractional tolerance to check step lenght is of similar lenght    
  double m_fracTolerance;
    
  /// Pointer to Geant4 Physical Volume of previous step
  G4VPhysicalVolume* m_prevVol;          
  /// Pointer to Geant4 Physical Volume of two previous steps
  G4VPhysicalVolume* m_prevPrevVol;
  /// Pointer to Geant4 Physical Volume of three previous steps
  G4VPhysicalVolume* m_prevPrevPrevVol;
    
};
 
} // closes namespace

#endif // GAUSSTOOLS_COMPONENTS_PINGPONGCUT_H
