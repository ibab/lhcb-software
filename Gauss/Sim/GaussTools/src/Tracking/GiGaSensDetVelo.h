// ============================================================================
#ifndef       GAUSSTOOLS_GiGaSensDetVelo_H
#define       GAUSSTOOLS_GiGaSensDetVelo_H 1 
// ============================================================================

/// GiGa 
#include "GiGa/GiGaSensDetBase.h"
#include "GaussTools/GaussTrackInformation.h"
#include "VeloHit.h"

/// forward declarations 
template <class SD> 
class GiGaSensDetFactory;

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


class GiGaSensDetVelo: virtual public GiGaSensDetBase
{
  /// friend factory 
  friend class GiGaFactory<GiGaSensDetVelo>;

protected:
  
  GiGaSensDetVelo
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaSensDetVelo();
  
public: 
 
  void GiGaSensDetVelo::Initialize(G4HCofThisEvent*HCE);

  virtual bool ProcessHits
  ( G4Step* step                , 
    G4TouchableHistory* history ) ;
  
private:
  ///
  GiGaSensDetVelo(); ///< no default constructor
  GiGaSensDetVelo( const GiGaSensDetVelo& ); ///< no copy constructor 
  GiGaSensDetVelo& operator=( const GiGaSensDetVelo& ) ; ///< no = 
  ///
private:
  ///
  VeloHitsCollection* veloCol;
};

// ============================================================================
#endif  ///< GAUSSTOOLS_GiGaSensDetVelo_H
// ============================================================================
