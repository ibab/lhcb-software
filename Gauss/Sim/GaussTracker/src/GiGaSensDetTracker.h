// $Id: GiGaSensDetTracker.h,v 1.6 2008-05-07 07:05:44 gcorti Exp $
#ifndef GIGASENSDETTRACKER_H
#define GIGASENSDETTRACKER_H 1 

// Include files
#include "GiGa/GiGaSensDetBase.h"
#include "GaussTools/GaussTrackInformation.h"
#include "TrackerHit.h"

// Forward declarations
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


/** @class GiGaSensDetTracker GiGaSensDetTracker.h
 *  
 *  Fill information in TrackerHits to be later stored
 *  in MCHits (entry point and displacement from exit point in default
 *  setup, displacement with momentum directions in alternative setup).
 *
 *  @author Witek POKORSKI
 *  @author Gloria CORTI
 *  @date   2006-07-17 (last revision)
 */
class GiGaSensDetTracker: virtual public GiGaSensDetBase
{

public:

  /// Standard constructor
  GiGaSensDetTracker( const std::string& type   ,
                      const std::string& name   ,
                      const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaSensDetTracker();
  

  /** Initialize method (Geant4).
   *  Called at the beginning of each event
   *  @see G4VSensitiveDetector
   *  @param HCE pointer to hit collection of current event
   */
  virtual void Initialize( G4HCofThisEvent* HCE );

  /** Process a hit (Geant4).
   *  The method is invoked by G4 for each step in the 
   *  sensitive detector. 
   *  This implementation stores information in TrackerHit
   *  to be later saved in MCHit for a tracker detector:
   *  the hits are saved for charged particles only and if
   *  the step and dE/dx in the step are non-zero.
   *  The default implementation stores entry point, 
   *  displacement (as exit-entry), dE/Dx in the step, 
   *  |P| of the particle and Time at entry point. 
   *  @see   G4VSensitiveDetector
   *  @param step     pointer to current Geant4 step 
   *  @param history  pointer to touchable history 
   */
  virtual bool ProcessHits( G4Step* step, 
                            G4TouchableHistory* history ) ;
  
private:

  GiGaSensDetTracker(); ///< no default constructor
  GiGaSensDetTracker( const GiGaSensDetTracker& ); ///< no copy constructor 
  GiGaSensDetTracker& operator=( const GiGaSensDetTracker& ) ; ///< no = 

  /// Pointer to G4 collection for this sensitive detector
  TrackerHitsCollection* m_trackerCol;

  /// Flag to store hits if dE/dx occured, this is to enable hits when
  /// switching it off for special studies (like with geantinos)
  bool m_requireEDep;
  
};


#endif  // GIGASENSDETTRACKER_H

