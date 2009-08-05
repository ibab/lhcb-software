// $Id: Track2EcalEAlg.cpp,v 1.5 2009-08-05 17:35:34 ibelyaev Exp $
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// Local
// ============================================================================
#include "CaloTrack2IDAlg.h"
// ============================================================================
/** @class Track2EcalEAlg Track2EcalEAlg.cpp
 *  preconfigured instance of class  CaloTrack2IDAlg
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-15
 */
class Track2EcalEAlg : public CaloTrack2IDAlg
{
  // ==========================================================================
  friend class AlgFactory<Track2EcalEAlg> ;
  // ==========================================================================
protected:  
  // ==========================================================================
  /// Standard protected constructor
  Track2EcalEAlg
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloTrack2IDAlg( name , pSvc ) 
  {
    if("HLT"==context() ){
      _setProperty ( "Output" , LHCb::CaloIdLocation::EcalEHlt   ) ;
      _setProperty ( "Filter" , LHCb::CaloIdLocation::InEcalHlt  ) ;
    }else{
      _setProperty ( "Output" , LHCb::CaloIdLocation::EcalE   ) ;
      _setProperty ( "Filter" , LHCb::CaloIdLocation::InEcal  ) ;
    }
    _setProperty ( "Tool"   , "EcalEnergyForTrack/EcalE"  ) ;
  } 
  /// virtual protected destructor 
  virtual ~Track2EcalEAlg(){} 
  // ==========================================================================
private:
  // ==========================================================================
  Track2EcalEAlg() ;
  Track2EcalEAlg           ( const Track2EcalEAlg& ) ;
  Track2EcalEAlg& operator=( const Track2EcalEAlg& ) ;
  // ==========================================================================
} ;
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( Track2EcalEAlg );
// ============================================================================
// The END 
// ============================================================================
