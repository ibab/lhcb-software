// ============================================================================
#ifndef       GIGA_GiGaSensDetTracker_H
#define       GIGA_GiGaSensDetTracker_H 1 
// ============================================================================
/// GiGa 
#include "GiGa/GiGaSensDetBase.h"
#include "TrackerHit.h"
/// forward declarations 
template <class SD> 
class GiGaSensDetFactory;

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/** @class GiGaSensDetTracker GiGaSensDetTracker.h GiGaSensDetTracker.h
 *
 *  Example of "primitive" implementation of sensitive detector
 *  just print some information inside G4 sensitive volume, no 
 *  real hit creation
 *
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */
class GiGaSensDetTracker: virtual public GiGaSensDetBase
{
  /// friend factory 
  friend class GiGaFactory<GiGaSensDetTracker>;

protected:
  
  /** standard constructor 
   *  @see GiGaSensDetBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaSensDetTracker
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaSensDetTracker();
  
public: 
 
  void GiGaSensDetTracker::Initialize(G4HCofThisEvent*HCE);

  /** process the hit
   *  @param step     pointer to current Geant4 step 
   *  @param history  pointert to touchable history 
   */
  virtual bool ProcessHits
  ( G4Step* step                , 
    G4TouchableHistory* history ) ;
  
private:
  ///
  GiGaSensDetTracker(); ///< no default constructor
  GiGaSensDetTracker( const GiGaSensDetTracker& ); ///< no copy constructor 
  GiGaSensDetTracker& operator=( const GiGaSensDetTracker& ) ; ///< no = 
  ///
private:
  ///
  TrackerHitsCollection *trackerCollection;
};

// ============================================================================
// The END 
// ============================================================================
#endif  ///< GIGA_GiGaSensDetTracker_H
// ============================================================================
