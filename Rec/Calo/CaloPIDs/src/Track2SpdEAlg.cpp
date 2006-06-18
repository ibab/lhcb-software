// $Id: Track2SpdEAlg.cpp,v 1.1 2006-06-18 18:35:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, verison $Revision: 1.1 $
// ============================================================================
// $Log:
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
  friend class AlgFactory<Track2SpdEAlg> ;
protected:  
  /// Standard protected constructor
  Track2SpdEAlg
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloTrack2IDAlg( name , pSvc ) 
  {
    setProperty ( "Output" , "Rec/Calo/SpdE"  ) ;
    setProperty ( "Filter" , LHCb::CaloIdLocation::InSpd  ) ;
    setProperty ( "Tool"   , "SpdEnergyForTrack/SpdE:PUBLIC"   ) ;
  } ;
  /// virtual protected destructor 
  virtual ~Track2SpdEAlg(){} ;  
private:
  Track2SpdEAlg() ;
  Track2SpdEAlg           ( const Track2SpdEAlg& ) ;
  Track2SpdEAlg& operator=( const Track2SpdEAlg& ) ;
} ;
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( Track2SpdEAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
