// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/System.h"
// ============================================================================
// Event/RecEvent 
// ============================================================================
#include "Event/RecVertex.h"
#include "Event/VertexBase.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Vertices3.h"
#include "LoKi/Constants.h"
#include "LoKi/Algs.h"
#include "LoKi/PhysKinematics.h"
#include "LoKi/ParticleProperties.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV ibelyaev@cern.ch
 *  @date   2010-12-05
 *   
 *                    $Revision$
 *  Last Modification $Date$
 *                 by $Author$
 */
// ============================================================================
// constructor from track cuts and "bad" value 
// ============================================================================
LoKi::Vertices::RecVertex2TrackMin::RecVertex2TrackMin
( const LoKi::Functor<const LHCb::Track*,double>& fun   , 
  const double                                    bad   ) 
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
  , m_fun   ( fun   ) 
  , m_bad   ( bad   )   
{}
// ============================================================================
// constructor from track cuts and "bad" value 
// ============================================================================
LoKi::Vertices::RecVertex2TrackMin::RecVertex2TrackMin
( const LoKi::Functor<const LHCb::Track*,double>& fun   )
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
  , m_fun   ( fun   ) 
  , m_bad   ( LoKi::Constants::PositiveInfinity ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::RecVertex2TrackMin::~RecVertex2TrackMin(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::RecVertex2TrackMin*
LoKi::Vertices::RecVertex2TrackMin::clone() const 
{ return new LoKi::Vertices::RecVertex2TrackMin(*this) ; }
// ============================================================================
// MANDATORY: the conly one essential method 
// ============================================================================
LoKi::Vertices::RecVertex2TrackMin::result_type 
LoKi::Vertices::RecVertex2TrackMin::operator()  
  ( LoKi::Vertices::RecVertex2TrackMin::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return 'bad'");
    return m_bad ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return 'bad'");
    return m_bad ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS ;
  const TRACKS& tracks = rv->tracks() ;
  //
  std::pair<TRACKS::const_iterator,double> r = 
    LoKi::Algs::extremum 
    ( tracks.begin ()     , 
      tracks.end   ()     , 
      m_fun               ,
      std::less<double>() ) ;
  //
  if ( tracks.end() == r.first ) {  return m_bad ; }
  //
  return r.second ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Vertices::RecVertex2TrackMin::fillStream
( std::ostream& s ) const
{ 
  s << "RV_TrMIN( " << m_fun ;
  if ( LoKi::Constants::PositiveInfinity != m_bad ) { s << "," << m_bad ; }
  return s << ")" ;
}  
// ============================================================================



// ============================================================================
// constructor from track cuts and "bad" value 
// ============================================================================
LoKi::Vertices::RecVertex2TrackMax::RecVertex2TrackMax
( const LoKi::Functor<const LHCb::Track*,double>& fun   , 
  const double                                    bad   )
  : LoKi::Vertices::RecVertex2TrackMin ( fun , bad ) 
{}
// ============================================================================
// constructor from track cuts and "bad" value 
// ============================================================================
LoKi::Vertices::RecVertex2TrackMax::RecVertex2TrackMax
( const LoKi::Functor<const LHCb::Track*,double>& fun   )
  : LoKi::Vertices::RecVertex2TrackMin ( fun , LoKi::Constants::NegativeInfinity ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::RecVertex2TrackMax::~RecVertex2TrackMax(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::RecVertex2TrackMax*
LoKi::Vertices::RecVertex2TrackMax::clone() const 
{ return new LoKi::Vertices::RecVertex2TrackMax(*this) ; }
// ============================================================================
// MANDATORY: the conly one essential method 
// ============================================================================
LoKi::Vertices::RecVertex2TrackMax::result_type 
LoKi::Vertices::RecVertex2TrackMax::operator()  
  ( LoKi::Vertices::RecVertex2TrackMax::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return 'bad'");
    return m_bad ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  //
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return 'bad'");
    return m_bad ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS ;
  const TRACKS& tracks = rv->tracks() ;
  //
  std::pair<TRACKS::const_iterator,double> r = 
    LoKi::Algs::extremum 
    ( tracks.begin ()        , 
      tracks.end   ()        , 
      m_fun                  ,
      std::greater<double>() ) ;
  //
  if ( tracks.end() == r.first ) {  return m_bad ; }
  //
  return r.second ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Vertices::RecVertex2TrackMax::fillStream
( std::ostream& s ) const
{ 
  s << "RV_TrMAX( " << m_fun ;
  if ( LoKi::Constants::NegativeInfinity != m_bad ) { s << "," << m_bad ; }
  return s << ")" ;
}  
// ============================================================================


// ============================================================================
// constructor from track cuts and "bad" value 
// ============================================================================
LoKi::Vertices::RecVertex2TrackSum::RecVertex2TrackSum
( const LoKi::Functor<const LHCb::Track*,double>& fun   , 
  const double                                    bad   )
  : LoKi::Vertices::RecVertex2TrackMax ( fun , bad ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::RecVertex2TrackSum::~RecVertex2TrackSum(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::RecVertex2TrackSum*
LoKi::Vertices::RecVertex2TrackSum::clone() const 
{ return new LoKi::Vertices::RecVertex2TrackSum(*this) ; }
// ============================================================================
// MANDATORY: the conly one essential method 
// ============================================================================
LoKi::Vertices::RecVertex2TrackSum::result_type 
LoKi::Vertices::RecVertex2TrackSum::operator()  
  ( LoKi::Vertices::RecVertex2TrackSum::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return 'bad'");
    return m_bad ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return 'bad'");
    return m_bad ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS ;
  const TRACKS& tracks = rv->tracks() ;
  //
  return 
    LoKi::Algs::accumulate 
    ( tracks.begin ()     , 
      tracks.end   ()     , 
      m_fun               ,
      0.0                 , 
      std::plus<double>() ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Vertices::RecVertex2TrackSum::fillStream
( std::ostream& s ) const
{ return s << "RV_TrSUM( " << m_fun << "," << m_bad << ")" ; }  
// ============================================================================


// ============================================================================
// constructor from track cuts, track index and "bad" value 
// ============================================================================
LoKi::Vertices::RecVertex2TrackFun::RecVertex2TrackFun
( const LoKi::Functor<const LHCb::Track*,double>& fun   , 
  const unsigned short                            index , 
  const double                                    bad   ) 
  : LoKi::Vertices::RecVertex2TrackSum ( fun , bad ) 
  , m_index ( index ) 
{}
// ============================================================================
// constructor from track cuts, track index and "bad" value 
// ============================================================================
LoKi::Vertices::RecVertex2TrackFun::RecVertex2TrackFun
( const LoKi::Functor<const LHCb::Track*,double>& fun   , 
  const unsigned short                            index )
  : LoKi::Vertices::RecVertex2TrackSum ( fun , LoKi::Constants::NegativeInfinity ) 
  , m_index ( index ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::RecVertex2TrackFun::~RecVertex2TrackFun(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::RecVertex2TrackFun*
LoKi::Vertices::RecVertex2TrackFun::clone() const 
{ return new LoKi::Vertices::RecVertex2TrackFun(*this) ; }
// ============================================================================
// MANDATORY: the conly one essential method 
// ============================================================================
LoKi::Vertices::RecVertex2TrackFun::result_type 
LoKi::Vertices::RecVertex2TrackFun::operator()  
  ( LoKi::Vertices::RecVertex2TrackFun::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return 'bad'");
    return m_bad ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return 'bad'");
    return m_bad ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS ;
  const TRACKS& tracks = rv->tracks() ;
  if ( tracks.size() <= m_index ) 
  {
    Error ("Invalid track index , return 'bad'");
    return m_bad ;
  }
  //
  // finally: use the function
  //
  return m_fun ( tracks [ m_index ] ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Vertices::RecVertex2TrackFun::fillStream
( std::ostream& s ) const
{
  s << "RV_TrFUN( " << m_fun << "," << m_index ;
  if ( LoKi::Constants::NegativeInfinity != m_bad ) { s << "," << m_bad ; }
  // 
  return s << ")" ; 
}
// ============================================================================



// ============================================================================
// constructor from track cuts
// ============================================================================
LoKi::Vertices::RecVertex2TrackHas::RecVertex2TrackHas
( const LoKi::Functor<const LHCb::Track*,bool>&   cut )
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate () 
  , m_cut   ( cut   ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::RecVertex2TrackHas::~RecVertex2TrackHas (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::RecVertex2TrackHas*
LoKi::Vertices::RecVertex2TrackHas::clone() const 
{ return new LoKi::Vertices::RecVertex2TrackHas(*this) ; }
// ============================================================================
// MANDATORY: the conly one essential method 
// ============================================================================
LoKi::Vertices::RecVertex2TrackHas::result_type 
LoKi::Vertices::RecVertex2TrackHas::operator()  
  ( LoKi::Vertices::RecVertex2TrackHas::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return 'false'");
    return false ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return 'false'");
    return false ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS ;
  const TRACKS& tracks = rv->tracks() ;
  //
  return LoKi::Algs::found ( tracks.begin () , 
                             tracks.end   () , m_cut ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Vertices::RecVertex2TrackHas::fillStream
( std::ostream& s ) const
{ return s << "RV_TrHAS( " << m_cut << ")" ; }





// ============================================================================
// constructor from track cuts, track index and "bad" value 
// ============================================================================
LoKi::Vertices::RecVertex2TrackCut::RecVertex2TrackCut
( const LoKi::Functor<const LHCb::Track*,bool>&   cut   , 
  const unsigned short                            index )
  : LoKi::Vertices::RecVertex2TrackHas ( cut ) 
  , m_index ( index )
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::RecVertex2TrackCut::~RecVertex2TrackCut(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::RecVertex2TrackCut*
LoKi::Vertices::RecVertex2TrackCut::clone() const 
{ return new LoKi::Vertices::RecVertex2TrackCut(*this) ; }
// ============================================================================
// MANDATORY: the conly one essential method 
// ============================================================================
LoKi::Vertices::RecVertex2TrackCut::result_type 
LoKi::Vertices::RecVertex2TrackCut::operator()  
  ( LoKi::Vertices::RecVertex2TrackCut::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return 'false'");
    return false ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return 'false'");
    return false ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS ;
  const TRACKS& tracks = rv->tracks() ;
  if ( tracks.size() <= m_index ) 
  {
    Error ("Invalid track index , return 'false'");
    return false ;
  }
  //
  // finally: use the function
  //
  return m_cut ( tracks [ m_index ] ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Vertices::RecVertex2TrackCut::fillStream
( std::ostream& s ) const
{ return s << "RV_TrCUT( " << m_cut << "," << m_index << ")" ; }
// ============================================================================




// ============================================================================
// constructor from track cuts and "bad" value 
// ============================================================================
LoKi::Vertices::RecVertex2TrackNum::RecVertex2TrackNum
( const LoKi::Functor<const LHCb::Track*,bool>& cut   , 
  const double                                  bad   ) 
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
  , m_cut   ( cut   ) 
  , m_bad   ( bad   )   
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::RecVertex2TrackNum::~RecVertex2TrackNum(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::RecVertex2TrackNum*
LoKi::Vertices::RecVertex2TrackNum::clone() const 
{ return new LoKi::Vertices::RecVertex2TrackNum(*this) ; }
// ============================================================================
// MANDATORY: the conly one essential method 
// ============================================================================
LoKi::Vertices::RecVertex2TrackNum::result_type 
LoKi::Vertices::RecVertex2TrackNum::operator()  
  ( LoKi::Vertices::RecVertex2TrackNum::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return 'bad'");
    return m_bad ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return 'bad'");
    return m_bad ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS ;
  const TRACKS& tracks = rv->tracks() ;
  //
  return LoKi::Algs::count_if 
    ( tracks.begin ()     , 
      tracks.end   ()     , 
      m_cut               ) ;

}// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Vertices::RecVertex2TrackNum::fillStream
( std::ostream& s ) const
{ 
  s << "RV_TrNUM( " << m_cut ;
  if ( -1 != m_bad ) { s << "," << m_bad ; }
  return s << ")" ;
}  
// ============================================================================


// ============================================================================
// constructor
// ============================================================================
LoKi::Vertices::RecVertexMomentum::RecVertexMomentum()
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
{}
// ============================================================================
// destructor
// ============================================================================
LoKi::Vertices::RecVertexMomentum::~RecVertexMomentum(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::RecVertexMomentum*
LoKi::Vertices::RecVertexMomentum::clone() const 
{ return new LoKi::Vertices::RecVertexMomentum(*this) ; }
// ============================================================================
// MANDATORY: the only one essential mehtod 
// ============================================================================
LoKi::Vertices::RecVertexMomentum::result_type 
LoKi::Vertices::RecVertexMomentum::operator() 
  ( LoKi::Vertices::RecVertexMomentum::argument v ) const
{
  //
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return 'InvalidMomentum'");
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return 'InvalidMomentum'");
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS ;
  const TRACKS& tracks = rv->tracks() ;
  //
  if ( tracks.empty() )
  {
    Warning ("Empty vertex, return 0") ;
    return 0 ;
  }
  //
  Gaudi::XYZVector mom ;
  //
  for ( TRACKS::const_iterator iTrack = tracks.begin() ; 
        tracks.end() != iTrack ; ++iTrack ) 
  {
    const LHCb::Track* track = *iTrack ;
    if ( 0 == track ) 
    {
      Warning ( "LHCb::Track* points to zero, skip it" ) ;
      continue ; 
    }
    mom += track->momentum() ;
  }
  //
  //
  return mom.R() ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Vertices::RecVertexMomentum::fillStream
( std::ostream& s ) const
{ return s << "RV_P " ; }  
// ============================================================================



// ============================================================================
// constructor
// ============================================================================
LoKi::Vertices::RecVertexPt::RecVertexPt()
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
{}
// ============================================================================
// destructor
// ============================================================================
LoKi::Vertices::RecVertexPt::~RecVertexPt(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::RecVertexPt*
LoKi::Vertices::RecVertexPt::clone() const 
{ return new LoKi::Vertices::RecVertexPt ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential mehtod 
// ============================================================================
LoKi::Vertices::RecVertexPt::result_type 
LoKi::Vertices::RecVertexPt::operator() 
  ( LoKi::Vertices::RecVertexPt::argument v ) const
{
  //
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return 'InvalidMomentum'");
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return 'InvalidMomentum'");
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS ;
  const TRACKS& tracks = rv->tracks() ;
  //
  if ( tracks.empty() )
  {
    Warning ( "Empty vertex, return 0" ) ;
    return 0 ;
  }
  //
  Gaudi::XYZVector mom ;
  //
  for ( TRACKS::const_iterator iTrack = tracks.begin() ; 
        tracks.end() != iTrack ; ++iTrack ) 
  {
    const LHCb::Track* track = *iTrack ;
    if ( 0 == track ) 
    {
      Warning ( "LHCb::Track* points to zero, skip it" ) ;
      continue ; 
    }
    mom += track->momentum() ;
  }
  //
  return mom.Rho () ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Vertices::RecVertexPt::fillStream
( std::ostream& s ) const
{ return s << "RV_PT " ; }  
// ============================================================================

  



  
// ============================================================================
// constructor from vector of masses 
// ============================================================================
LoKi::Vertices::RecVertexMass::RecVertexMass 
( const std::vector<double>&           masses ) 
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
  , m_masses ( masses ) 
{}
// ============================================================================
// constructor from vector of IDS 
// ============================================================================
LoKi::Vertices::RecVertexMass::RecVertexMass 
( const std::vector<LHCb::ParticleID>& ids    ) 
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
  , m_masses () 
{
  for ( std::vector<LHCb::ParticleID>::const_iterator ipid = ids.begin() ; 
        ids.end() != ipid ; ++ipid ) 
  { m_masses.push_back ( LoKi::Particles::massFromPID ( *ipid ) ) ; }
  //
}
// ============================================================================
// constructor from vector of IDS 
// ============================================================================
LoKi::Vertices::RecVertexMass::RecVertexMass 
( const std::vector<std::string>& ids    ) 
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
  , m_masses () 
{
  //
  for ( std::vector<std::string>::const_iterator ipid = ids.begin() ; 
        ids.end() != ipid ; ++ipid ) 
  { m_masses.push_back ( LoKi::Particles::massFromName ( *ipid ) ) ; }
  //
}
// ============================================================================
// constructor from IDS 
// ============================================================================
LoKi::Vertices::RecVertexMass::RecVertexMass 
( const std::string& id1 , 
  const std::string& id2 ) 
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
  , m_masses () 
{
  //
  m_masses.push_back ( LoKi::Particles::massFromName ( id1 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromName ( id2 ) ) ;  
  //
}
// ============================================================================
// constructor from IDS 
// ============================================================================
LoKi::Vertices::RecVertexMass::RecVertexMass 
( const std::string& id1 , 
  const std::string& id2 ,
  const std::string& id3 ) 
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
  , m_masses () 
{
  //
  m_masses.push_back ( LoKi::Particles::massFromName ( id1 ) ) ;
  m_masses.push_back ( LoKi::Particles::massFromName ( id2 ) ) ;  
  m_masses.push_back ( LoKi::Particles::massFromName ( id3 ) ) ;  
  //
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Vertices::RecVertexMass::~RecVertexMass () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::RecVertexMass*
LoKi::Vertices::RecVertexMass::clone() const 
{ return new LoKi::Vertices::RecVertexMass ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential mehtod 
// ============================================================================
LoKi::Vertices::RecVertexMass::result_type 
LoKi::Vertices::RecVertexMass::operator() 
  ( LoKi::Vertices::RecVertexMass::argument v ) const 
{
  //
  if ( 0 == v ) 
  {
    Error ("LHCb::VertexBase* points to NULL, return 'InvalidMass'");
    return LoKi::Constants::InvalidMass ;
  }
  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return 'InvalidMass'");
    return LoKi::Constants::InvalidMass ;
  }
  //
  typedef SmartRefVector<LHCb::Track> TRACKS ;
  const TRACKS& tracks = rv->tracks() ;
  //
  if ( tracks.size() != m_masses.size() )
  { Warning("size(Tracks)!=size(Masses)") ; }
  //                                           
  if      ( 2 == tracks.size() && 2 <= m_masses.size() ) 
  {
    return LoKi::PhysKinematics::mass 
      ( tracks [0] , m_masses [0] ,
        tracks [1] , m_masses [1] ) ;
    
  }
  else if ( 3 == tracks.size() && 3 <= m_masses.size() ) 
  {
    return LoKi::PhysKinematics::mass 
      ( tracks [0] , m_masses [0] ,
        tracks [1] , m_masses [1] ,
        tracks [2] , m_masses [2] ) ;
    
  }
  //
  return LoKi::PhysKinematics::mass ( tracks , m_masses ) ;
  //
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Vertices::RecVertexMass::fillStream
( std::ostream& s ) const
{ 
  s << "RV_MASS(" ;
  Gaudi::Utils::toStream ( m_masses , s ) ;
  return s << ")" ;
}  
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
