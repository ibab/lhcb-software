// ============================================================================
#ifndef       GAUSSTOOLS_GiGaSensDetMuon_H
#define       GAUSSTOOLS_GiGaSensDetMuon_H 1 
// ============================================================================

/// GiGa 
#include "GiGa/GiGaSensDetBase.h"
#include "GaussTools/GiGaTrackInformation.h"
#include "MuonHit.h"

/// forward declarations 
template <class SD> 
class GiGaSensDetFactory;

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


class GiGaSensDetMuon: virtual public GiGaSensDetBase
{
  /// friend factory 
  friend class GiGaFactory<GiGaSensDetMuon>;

protected:
  
  GiGaSensDetMuon
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaSensDetMuon();
  
public: 
 
  void GiGaSensDetMuon::Initialize(G4HCofThisEvent*HCE);

  virtual bool ProcessHits
  ( G4Step* step                , 
    G4TouchableHistory* history ) ;
  
private:
  ///
  GiGaSensDetMuon(); ///< no default constructor
  GiGaSensDetMuon( const GiGaSensDetMuon& ); ///< no copy constructor 
  GiGaSensDetMuon& operator=( const GiGaSensDetMuon& ) ; ///< no = 
  ///
private:
  ///
  MuonHitsCollection* muonCol;
};

// ============================================================================
#endif  ///< GAUSSTOOLS_GiGaSensDetMuon_H
// ============================================================================
