// ============================================================================
// Local
// ============================================================================
#include "CaloTrack2IDAlg.h"
// ============================================================================
/** @class Track2SpdEAlg Track2SpdEAlg.cpp
 *  preconfigured instance of class  CaloTrack2IDAlg
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-15
 */
class Track2SpdEAlg : public CaloTrack2IDAlg
{
  // ==========================================================================
  friend class AlgFactory<Track2SpdEAlg> ;
  // ==========================================================================
protected:  
  // ==========================================================================
  /// Standard protected constructor
  Track2SpdEAlg
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloTrack2IDAlg( name , pSvc ) 
  {

    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Output" , CaloIdLocation( "SpdE" , context() )  ) ;
    _setProperty ( "Filter" , CaloIdLocation( "InSpd", context() )  ) ;  

    _setProperty ( "Tool"   , "SpdEnergyForTrack/SpdE"   ) ;
  } 
  /// virtual protected destructor 
  virtual ~Track2SpdEAlg(){} 
  // ==========================================================================
private:
  // ==========================================================================
  Track2SpdEAlg() ;
  Track2SpdEAlg           ( const Track2SpdEAlg& ) ;
  Track2SpdEAlg& operator=( const Track2SpdEAlg& ) ;
  // ==========================================================================
} ;
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( Track2SpdEAlg )
// ============================================================================
// The END 
// ============================================================================
