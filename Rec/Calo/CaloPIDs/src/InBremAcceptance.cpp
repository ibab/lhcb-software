// $Id: InBremAcceptance.cpp,v 1.1 2006-06-18 18:35:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptance.h"
// ============================================================================
/** @class In BremAcceptance 
 *  The precofigured instance of InCaloAcceptance Tool
 *  @author Victor EGORYCHEV Victor.Egorychev@cern.ch
 *  @author Vanya  BELYAEV    ibelyaev@phsycis.syr.edu
 *  @date   2006-05-28
 */
// ============================================================================
class InBremAcceptance : public InCaloAcceptance 
{  
  /// friend factory for instantiation 
  friend class ToolFactory<InBremAcceptance> ; 
public:
  /** check the track is in acceptance of given calorimeter 
   *  @see IInAcceptance
   *  @param  track track to be checked
   *  @return true if the track is in acceptance
   */
  virtual bool inAcceptance ( const LHCb::Track* track ) const ;
protected:
  /// standard protected constructor 
  InBremAcceptance
  ( const std::string& type   ,
    const std::string& name   , 
    const IInterface*  parent )
    : InCaloAcceptance( type, name , parent )
  {
    setProperty ( "Extrapolator" , "TrackLinearExtrapolator/Linear:PUBLIC" ) ;
    setProperty ( "Calorimeter"  , DeCalorimeterLocation::Ecal ) ;
  } ;
  /// virtual and protected destructor 
  virtual ~InBremAcceptance(){} ;
private:
  InBremAcceptance() ;
  InBremAcceptance            ( const InBremAcceptance& ) ;
  InBremAcceptance& operator= ( const InBremAcceptance& ) ;  
} ;
// ============================================================================
/// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( InBremAcceptance ) ;
// ============================================================================
/// check the expected bremstrahlung photon is in acceptance of Ecal 
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
  { state = CaloTrackTool::state ( *track , LHCb::State::BegRich1 ) ; }
  if ( 0 == state ) 
  { state = CaloTrackTool::state ( *track , LHCb::State::EndVelo  ) ; }
  // no appropriate state is found 
  Assert ( 0 != state , "No appropriate staee is found " ) ;
  // get the line form the state  
  const Line l = line ( *state ) ;
  // get the point of intersection of the line with the plane 
  Gaudi::XYZPoint point ;
  double mu = 0 ;
  Gaudi::Math::intersection ( l , plane() , point , mu ) ;
  //
  return ok ( point ) ;
} ;

// ============================================================================
// The END 
// ============================================================================
