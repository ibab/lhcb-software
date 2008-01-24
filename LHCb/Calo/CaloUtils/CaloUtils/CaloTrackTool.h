// $Id: CaloTrackTool.h,v 1.7 2008-01-24 10:16:53 vegorych Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
#ifndef CALOUTILS_CALO_CALOTRACKTOOL_H 
#define CALOUTILS_CALO_CALOTRACKTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// STD&STL 
// ============================================================================
#include <vector>
#include <string>
#include <algorithm>
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/Line.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
#include "Event/TrackFunctor.h"
#include "Event/State.h"
#include "Event/TrackUse.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackMatch.h"
// ============================================================================
// TrackInterfaces 
// ============================================================================
#include "TrackInterfaces/ITrackExtrapolator.h"
// ============================================================================
// CaloDet 
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================

namespace Calo
{
  /** @class Calo::CaloTrackTool CaloTrackTool.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-05-28
   */
  class CaloTrackTool : public GaudiTool 
  {
  public:
    typedef std::vector<LHCb::Track::Types>               TrackTypes ;
    typedef Gaudi::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line       ;
  public:
    /// initialize the tool 
    virtual StatusCode initialize () ;

  protected:
    /// standard constructor 
    CaloTrackTool
    ( const std::string& type   , // ?
      const std::string& name   , 
      const IInterface*  parent ) ;
    /// protected destructor 
    virtual ~CaloTrackTool() {}
  private:
    /// the default constructor is disabled ;
    CaloTrackTool() ;
  protected:
    inline ITrackExtrapolator* extrapolator      () const ;
    inline ITrackExtrapolator* fastExtrapolator  () const ;
  protected:
    /// Propagate track to a given 3D-place 
    inline StatusCode propagate 
    ( const LHCb::Track&      track ,
      const Gaudi::Plane3D&   plane ,
      LHCb::State&            state ,
      const LHCb::ParticleID& pid   = LHCb::ParticleID(211) ) const ;
    /// Propagate state to a given 3D-place 
    inline StatusCode propagate 
    ( LHCb::State&            state ,
      const Gaudi::Plane3D&   plane ,
      const LHCb::ParticleID& pid   = LHCb::ParticleID(211) ) const ;
    /// Propagate state to a given Z 
    inline StatusCode propagate 
    ( LHCb::State&            state ,
      const double            z     ,
      const LHCb::ParticleID& pid   = LHCb::ParticleID(211) ) const ;
  protected:
    /// construct the straight line from the state 
    inline Line line ( const LHCb::State& state ) const 
    { return Line ( state.position() , state.slopes () ) ; } ;
  protected:
    /** get  a pointer to the satte for the given track at given location 
     *  it shodul be faster then double usage of 
     *  LHCb::Track::hasStateAt ( location )  and LHCb::stateAt ( location ) 
     *  In addition it scans the list of states fro the end - 
     *  it is good for calorimeter 
     */
    inline const LHCb::State* state 
    ( const LHCb::Track&          track , 
      const LHCb::State::Location loc   ) const ;
    /** get  a pointer to the satte for the given track at given location 
     *  it shodul be faster then double usage of 
     *  LHCb::Track::hasStateAt ( location )  and LHCb::stateAt ( location ) 
     *  In addition it scans the list of states fro the end - 
     *  it is good for calorimeter 
     */
    inline       LHCb::State* state 
    ( LHCb::Track&                track , 
      const LHCb::State::Location loc   ) const ;
  protected:
    /// check if the track to be used @see TrackUse 
    inline bool use  ( const LHCb::Track* track ) const { return m_use( track) ; } 
    /// print the short infomration about track flags 
    inline MsgStream& print
    ( MsgStream&         stream , 
      const LHCb::Track* track  ) const ;
    /// print the short infomration about track flags 
    inline MsgStream& print 
    ( const LHCb::Track* track             , 
      const MSG::Level   level = MSG::INFO ) const ;
  protected:
    double tolerance() const { return m_tolerance ; }
    const std::string& detectorName() const { return m_detectorName ; }
    const DeCalorimeter* calo() const { return m_calo ; }
  private:
    // extrapolator name 
    std::string              m_extrapolatorName     ; ///< extrapolator name 
    // the actual extrapolator 
    mutable ITrackExtrapolator* m_extrapolator ; ///< the actual extrapolator
    // fast extrapolator name 
    std::string              m_fastExtrapolatorName ; ///< extrapolator name 
    // the actual extrapolator 
    mutable ITrackExtrapolator* m_fastExtrapolator; ///< the fast extrapolator
    // z-position of "linear" extrapolation 
    double                   m_fastZ                ;
    // plane extrapolation tolerance
    double                   m_tolerance            ;
    // plane extrapolation angular tolerance 
    double                   m_cosTolerance         ;
    // masimal number of iterations 
    unsigned int             m_maxIter              ;
    // check the tracks? 
    bool                     m_checkTrack           ;
    // detector element 
    const DeCalorimeter*     m_calo                 ;
    std::string              m_detectorName         ;
    // track selector 
    TrackUse                 m_use                  ;
    // local storages 
    mutable Gaudi::XYZPoint  m_pos                  ;
    mutable Gaudi::XYZVector m_mom                  ;    
  } ;
  
} // end of namespace Calo
// ============================================================================
/// get the extrapolator 
// ============================================================================
inline ITrackExtrapolator* 
Calo::CaloTrackTool::extrapolator() const 
{
  if ( 0 != m_extrapolator     ) { return m_extrapolator ; }
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName , this ) ;
  return m_extrapolator ;
} ;
// ============================================================================
/// get the fast extrapolator 
// ============================================================================
inline ITrackExtrapolator* 
Calo::CaloTrackTool::fastExtrapolator() const 
{
  if ( 0 != m_fastExtrapolator ) { return m_fastExtrapolator ; }
  m_fastExtrapolator = 
    tool<ITrackExtrapolator>( m_fastExtrapolatorName , this ) ;
  return m_fastExtrapolator ;
} ;
// ============================================================================
/// Propagate track to a given 3D-place 
// ============================================================================
inline StatusCode 
Calo::CaloTrackTool::propagate 
( const LHCb::Track&      track ,
  const Gaudi::Plane3D&   plane ,
  LHCb::State&            state ,
  const LHCb::ParticleID& pid   ) const 
{
  state = track.closestState ( plane ) ;
  if ( ::fabs( plane.Distance ( state.position() ) ) < tolerance() ) 
  { return StatusCode::SUCCESS ; }
  return propagate ( state , plane , pid ) ; 
} ;
// ============================================================================
/// Propagate state to a given 3D-place 
// ============================================================================
inline StatusCode 
Calo::CaloTrackTool::propagate 
( LHCb::State&            state ,
  const Gaudi::Plane3D&   plane ,
  const LHCb::ParticleID& pid   ) const 
{
  // check the plane: if it is "almost Z=const"-plane
  const Gaudi::XYZVector& normal = plane.Normal() ;
  if ( m_cosTolerance < ::fabs ( normal.Z() ) )  
  {
    // use the standard method 
    const double Z =  -1*plane.HesseDistance()/normal.Z() ;
    return propagate (  state , Z , pid ) ;
  }
  Gaudi::XYZPoint point ;
  for ( unsigned int iter = 0 ; iter < m_maxIter ; ++iter ) 
  {
    const double distance = ::fabs ( plane.Distance( state.position() ) );
    if ( distance <m_tolerance ) { return StatusCode::SUCCESS ; }   // RETURN 
    double mu = 0.0 ;
    if ( !Gaudi::Math::intersection ( line ( state ) , plane , point , mu ) )
    { return Error ( "propagate: line does not cross the place" ) ; }  // RETURN 
    StatusCode sc = propagate ( state , point.Z() , pid ) ;
    if ( sc.isFailure() ) 
    { return Error ( "propagate: failure from propagate" , sc  ) ; }   // RETURN
  } ;
  return Error ( "propagate: no convergency has been reached" ) ;
} ;
// ============================================================================
/// Propagate state to a given Z 
// ============================================================================
inline StatusCode 
Calo::CaloTrackTool::propagate 
( LHCb::State&            state ,
  const double            z     ,
  const LHCb::ParticleID& pid   ) const 
{
  if      ( std::max ( state.z() , z ) <  m_fastZ ) 
  { // use the regular extrapolator 
    return extrapolator     () -> propagate ( state , z , pid ) ;
  }
  else if ( std::min ( state.z() , z ) > m_fastZ  ) 
  { // use the fast (linear) extrapolator 
    return fastExtrapolator () -> propagate ( state , z , pid ) ;
  }
  // use the pair of extrapolators 
  StatusCode sc1 ;
  StatusCode sc2 ;
  if ( state.z () < z ) 
  {
    sc1 = extrapolator     () -> propagate ( state  , m_fastZ , pid ) ; 
    sc2 = fastExtrapolator () -> propagate ( state  , z       , pid ) ;
  }
  else 
  {
    sc2 = fastExtrapolator () -> propagate ( state  , m_fastZ , pid ) ;
    sc1 = extrapolator     () -> propagate ( state  , z       , pid ) ;
  }
  if ( sc1.isFailure() ) 
  { return Error ("Error from extrapolator"     , sc1 ) ; }
  if ( sc2.isFailure() ) 
  { return Error ("Error form FastExtrapolator" , sc2 ) ; }
  //
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// print the short infomration about track flags 
// ============================================================================
inline MsgStream& 
Calo::CaloTrackTool::print 
( MsgStream&         stream , 
  const LHCb::Track* track  ) const 
{ return stream.isActive() ? m_use.print ( stream , track ) : stream ; }
// ============================================================================
/// print the short infomration about track flags 
// ============================================================================
inline MsgStream& 
Calo::CaloTrackTool::print 
( const LHCb::Track* track , 
  const MSG::Level   level ) const 
{ return print ( msgStream ( level ) , track ) ; }
// ============================================================================
/// get  a pointer to the state for the given track at given location 
// ============================================================================
inline const LHCb::State* 
Calo::CaloTrackTool::state 
( const LHCb::Track& track , const LHCb::State::Location loc   ) const 
{ 
  typedef std::vector<LHCb::State*> _States ;
  const _States& states = track.states()  ;
  // loop in reverse order: for calo should be a bit more efficient 
  _States::const_reverse_iterator found = std::find_if 
    ( states.rbegin() , states.rend(),
      TrackFunctor::HasKey<LHCb::State,const LHCb::State::Location&>
      ( &LHCb::State::checkLocation , loc ) ) ;
  //
  if ( states.rend() == found ) { return 0 ;}                  // RETURN 
  return *found ;                                              // RETURN 
} ;
// ============================================================================
/// get  a pointer to the state for the given track at given location 
// ============================================================================
inline  LHCb::State* 
Calo::CaloTrackTool::state 
( LHCb::Track& track , const LHCb::State::Location loc   ) const 
{
  const LHCb::Track& _t = track ;
  // yes, I know that const_cast is bad, but the code duplication is worse
  return const_cast<LHCb::State*> ( state ( _t , loc ) ) ;
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOUTILS_CALO_CALOTRACKTOOL_H
// ============================================================================
