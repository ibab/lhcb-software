// $Id: Track2HcalEAlg.cpp,v 1.2 2007-03-02 15:22:08 cattanem Exp $
// ============================================================================
// Local
// ============================================================================
#include "CaloTrack2IDAlg.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
/** @class Track2HcalEAlg Track2HcalEAlg.cpp
 *  preconfigured instance of class  CaloTrack2IDAlg
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-15
 */
class Track2HcalEAlg : public CaloTrack2IDAlg
{
  friend class AlgFactory<Track2HcalEAlg> ;
protected:  
  /// Standard protected constructor
  Track2HcalEAlg
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloTrack2IDAlg( name , pSvc ) 
  {
    setProperty ( "Output" , LHCb::CaloIdLocation::HcalE  ) ;
    setProperty ( "Filter" , LHCb::CaloIdLocation::InHcal ) ;
    setProperty ( "Tool"   , "HcalEnergyForTrack/HcalE:PUBLIC"  ) ;
  } ;
  /// virtual protected destructor 
  virtual ~Track2HcalEAlg(){} ;  
private:
  Track2HcalEAlg() ;
  Track2HcalEAlg           ( const Track2HcalEAlg& ) ;
  Track2HcalEAlg& operator=( const Track2HcalEAlg& ) ;
} ;
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( Track2HcalEAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
