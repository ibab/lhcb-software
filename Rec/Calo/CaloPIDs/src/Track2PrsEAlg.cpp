// $Id: Track2PrsEAlg.cpp,v 1.2 2007-03-02 15:22:08 cattanem Exp $
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// Local
// ============================================================================
#include "CaloTrack2IDAlg.h"
// ============================================================================
/** @class Track2PrsEAlg Track2PrsEAlg.cpp
 *  preconfigured instance of class  CaloTrack2IDAlg
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-15
 */
class Track2PrsEAlg : public CaloTrack2IDAlg
{
  friend class AlgFactory<Track2PrsEAlg> ;
protected:  
  /// Standard protected constructor
  Track2PrsEAlg
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloTrack2IDAlg( name , pSvc ) 
  {
    setProperty ( "Output" , LHCb::CaloIdLocation::PrsE   ) ;
    setProperty ( "Filter" , LHCb::CaloIdLocation::InPrs  ) ;
    setProperty ( "Tool"   , "PrsEnergyForTrack/PrsE:PUBLIC"   ) ;
  } ;
  /// virtual protected destructor 
  virtual ~Track2PrsEAlg(){} ;  
private:
  Track2PrsEAlg() ;
  Track2PrsEAlg           ( const Track2PrsEAlg& ) ;
  Track2PrsEAlg& operator=( const Track2PrsEAlg& ) ;
} ;
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( Track2PrsEAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
