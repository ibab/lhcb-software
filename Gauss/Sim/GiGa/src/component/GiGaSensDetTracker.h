// ============================================================================
#ifndef       GIGA_GiGaSensDetTracker_H
#define       GIGA_GiGaSensDetTracker_H 1 
// ============================================================================

/// GiGa 
#include "GiGa/GiGaSensDetBase.h"
#include "GiGa/TrackerHit.h"

/// forward declarations 
template <class SD> 
class GiGaSensDetFactory;

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


class GiGaSensDetTracker: virtual public GiGaSensDetBase
{
  /// friend factory 
  friend class GiGaFactory<GiGaSensDetTracker>;

protected:
  
  GiGaSensDetTracker
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaSensDetTracker();
  
public: 
 
  void GiGaSensDetTracker::Initialize(G4HCofThisEvent*HCE);

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
#endif  ///< GIGA_GiGaSensDetTracker_H
// ============================================================================
