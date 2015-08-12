// ============================================================================
// Include files 
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptance.h"
#include "Linear.h"
// ============================================================================
/** @class InBremAcceptance 
 *  The preconfigured instance of InCaloAcceptance Tool
 *  @author Victor EGORYCHEV Victor.Egorychev@cern.ch
 *  @author Vanya  BELYAEV    ibelyaev@phsycis.syr.edu
 *  @date   2006-05-28
 */
// ============================================================================
class InBremAcceptance : public InCaloAcceptance 
{ 
  // ==========================================================================
  /// friend factory for instantiation 
  friend class ToolFactory<InBremAcceptance> ; 
  // ==========================================================================
public:
  // ==========================================================================
  /** check the track is in acceptance of given calorimeter 
   *  @see IInAcceptance
   *  @param  track track to be checked
   *  @return true if the track is in acceptance
   */
  virtual bool inAcceptance ( const LHCb::Track* track ) const ;
  // ==========================================================================
protected:
  // ==========================================================================
  /// standard protected constructor 
  InBremAcceptance
  ( const std::string& type   ,
    const std::string& name   , 
    const IInterface*  parent )
    : InCaloAcceptance( type, name , parent )
  {
    _setProperty ( "Extrapolator" , "TrackLinearExtrapolator/Linear" ) ;
    _setProperty ( "Calorimeter"  , DeCalorimeterLocation::Ecal ) ;
  } 
  /// virtual and protected destructor 
  virtual ~InBremAcceptance(){} 
  // ==========================================================================
private:
  // ==========================================================================
  InBremAcceptance() ;
  InBremAcceptance            ( const InBremAcceptance& ) ;
  InBremAcceptance& operator= ( const InBremAcceptance& ) ;  
  // ==========================================================================
} ;
// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( InBremAcceptance )
// ============================================================================
// check the expected bremstrahlung photon is in acceptance of Ecal 
// ============================================================================
bool InBremAcceptance::inAcceptance ( const LHCb::Track* track) const 
{ 
  // check the goodness of the tarck 
  if ( !use ( track ) ) { return false ; }                          // RETURN
  // 
  // find the appropriate state 
  const LHCb::State* state =  0 ;
  if ( 0 == state ) 
  { state = CaloTrackTool::state ( *track , LHCb::State::AtTT     ) ; }
  if ( 0 == state ) 
  { state = CaloTrackTool::state ( *track , LHCb::State::EndRich1 ) ; }
  if ( 0 == state ) 
  { state = CaloTrackTool::state ( *track , LHCb::State::BegRich1 ) ; }
  if ( 0 == state ) 
  { state = CaloTrackTool::state ( *track , LHCb::State::EndVelo  ) ; }
  if ( 0 == state ) 
  { 
    // get the closest state to some artificial value  
    state = &(track->closestState( 2.0 * Gaudi::Units::meter ) ) ;
    // allowed z ?
    if ( state->z() > 4.0 * Gaudi::Units::meter ) 
    {
      Error ( "No appropriate states are found, see 'debug'").ignore() ; 
      if (msgLevel(MSG::DEBUG)) print ( debug() , track ) ;
      return false ; 
    }
  }
  // get the line form the state  
  const Line l = line ( *state ) ;
  // get the point of intersection of the line with the plane 
  Gaudi::XYZPoint point ;
  double mu = 0 ;
  Gaudi::Math::intersection ( l , plane() , point , mu ) ;
  //
  return ok ( point ) ;
}
// ============================================================================
// The END 
// ============================================================================
