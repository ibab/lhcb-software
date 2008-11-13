// $Id: VxMaker.cpp,v 1.1 2008-11-13 13:14:42 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <memory>
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Combiner.h"
#include "LoKi/VxMaker.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hlt1::VxMaker
 *  @date 2008-11-11 
 *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
 */
// ============================================================================
// constructor from two track sources & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& tr1     , 
  const LoKi::Hlt1::VxMaker::TrSource& tr2     , 
  const stringKey&                     key     ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( tr1 ) 
  , m_track2 ( tr2 ) 
  , m_same   ( &tr1 == &tr2 ) 
  , m_cut2tr ( LoKi::Constant<LoKi::TrackTypes::TrackPair,bool>( true ) ) 
  , m_cut2tr_trivial ( true ) 
  , m_cut4rv ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
  , m_cut4rv_trivial ( true )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from two track sources & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& tr1     , 
  const LoKi::Hlt1::VxMaker::TrSource& tr2     , 
  const stringKey&                     key     ,
  const LoKi::Types::RVCuts&           cuts4rv ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( tr1 ) 
  , m_track2 ( tr2 ) 
  , m_same   ( &tr1 == &tr2 ) 
  , m_cut2tr ( LoKi::Constant<LoKi::TrackTypes::TrackPair,bool>( true ) ) 
  , m_cut2tr_trivial ( true  ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from two track sources & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& tr1     , 
  const LoKi::Hlt1::VxMaker::TrSource& tr2     , 
  const stringKey&                     key     ,
  const LoKi::Types::TTrCuts&          cuts2tr ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( tr1 ) 
  , m_track2 ( tr2 ) 
  , m_same   ( &tr1 == &tr2 ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
  , m_cut4rv_trivial ( true  )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from two track sources & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& tr1     , 
  const LoKi::Hlt1::VxMaker::TrSource& tr2     , 
  const stringKey&                     key     ,
  const LoKi::Types::TTrCuts&          cuts2tr ,
  const LoKi::Types::RVCuts&           cuts4rv ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( tr1 ) 
  , m_track2 ( tr2 ) 
  , m_same   ( &tr1 == &tr2 ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from two track sources & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& tr1     , 
  const LoKi::Hlt1::VxMaker::TrSource& tr2     , 
  const stringKey&                     key     ,
  const LoKi::Types::RVCuts&           cuts4rv ,
  const LoKi::Types::TTrCuts&          cuts2tr )
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( tr1 ) 
  , m_track2 ( tr2 ) 
  , m_same   ( &tr1 == &tr2 ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from two track sources & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& tr1     , 
  const LoKi::Hlt1::VxMaker::TrSource& tr2     , 
  const std::string&                   key     ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( tr1 ) 
  , m_track2 ( tr2 ) 
  , m_same   ( &tr1 == &tr2 ) 
  , m_cut2tr ( LoKi::Constant<LoKi::TrackTypes::TrackPair,bool>( true ) ) 
  , m_cut2tr_trivial ( true ) 
  , m_cut4rv ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
  , m_cut4rv_trivial ( true )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from two track sources & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& tr1     , 
  const LoKi::Hlt1::VxMaker::TrSource& tr2     , 
  const std::string&                   key     ,
  const LoKi::Types::RVCuts&           cuts4rv ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( tr1 ) 
  , m_track2 ( tr2 ) 
  , m_same   ( &tr1 == &tr2 ) 
  , m_cut2tr ( LoKi::Constant<LoKi::TrackTypes::TrackPair,bool>( true ) ) 
  , m_cut2tr_trivial ( true  ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from two track sources & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& tr1     , 
  const LoKi::Hlt1::VxMaker::TrSource& tr2     , 
  const std::string&                   key     ,
  const LoKi::Types::TTrCuts&          cuts2tr ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( tr1 ) 
  , m_track2 ( tr2 ) 
  , m_same   ( &tr1 == &tr2 ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
  , m_cut4rv_trivial ( true  )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from two track sources & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& tr1     , 
  const LoKi::Hlt1::VxMaker::TrSource& tr2     , 
  const std::string&                   key     ,
  const LoKi::Types::TTrCuts&          cuts2tr ,
  const LoKi::Types::RVCuts&           cuts4rv ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( tr1 ) 
  , m_track2 ( tr2 ) 
  , m_same   ( &tr1 == &tr2 ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from two track sources & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& tr1     , 
  const LoKi::Hlt1::VxMaker::TrSource& tr2     , 
  const std::string&                   key     ,
  const LoKi::Types::RVCuts&           cuts4rv ,
  const LoKi::Types::TTrCuts&          cuts2tr )
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( tr1 ) 
  , m_track2 ( tr2 ) 
  , m_same   ( &tr1 == &tr2 ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from one track source & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& trk     , 
  const stringKey&                     key     ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( trk ) 
  , m_track2 ( trk ) 
  , m_same   ( true ) 
  , m_cut2tr ( LoKi::Constant<LoKi::TrackTypes::TrackPair,bool>( true ) ) 
  , m_cut2tr_trivial ( true ) 
  , m_cut4rv ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
  , m_cut4rv_trivial ( true )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from one track source & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& trk     , 
  const stringKey&                     key     ,
  const LoKi::Types::RVCuts&           cuts4rv ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( trk ) 
  , m_track2 ( trk ) 
  , m_same   ( true ) 
  , m_cut2tr ( LoKi::Constant<LoKi::TrackTypes::TrackPair,bool>( true ) ) 
  , m_cut2tr_trivial ( true  ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from one track source & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& trk     , 
  const stringKey&                     key     ,
  const LoKi::Types::TTrCuts&          cuts2tr ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( trk ) 
  , m_track2 ( trk ) 
  , m_same   ( true ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
  , m_cut4rv_trivial ( true  )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from one track source & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& trk     , 
  const stringKey&                     key     ,
  const LoKi::Types::TTrCuts&          cuts2tr ,
  const LoKi::Types::RVCuts&           cuts4rv ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( trk ) 
  , m_track2 ( trk ) 
  , m_same   ( true ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from one track source & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& trk     , 
  const stringKey&                     key     ,
  const LoKi::Types::RVCuts&           cuts4rv ,
  const LoKi::Types::TTrCuts&          cuts2tr )
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( trk ) 
  , m_track2 ( trk ) 
  , m_same   ( true ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from one track source & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& trk     , 
  const std::string&                   key     ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( trk ) 
  , m_track2 ( trk ) 
  , m_same   ( true ) 
  , m_cut2tr ( LoKi::Constant<LoKi::TrackTypes::TrackPair,bool>( true ) ) 
  , m_cut2tr_trivial ( true ) 
  , m_cut4rv ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
  , m_cut4rv_trivial ( true )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from one track source & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& trk     , 
  const std::string&                   key     ,
  const LoKi::Types::RVCuts&           cuts4rv ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( trk ) 
  , m_track2 ( trk ) 
  , m_same   ( true ) 
  , m_cut2tr ( LoKi::Constant<LoKi::TrackTypes::TrackPair,bool>( true ) ) 
  , m_cut2tr_trivial ( true  ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from one track source & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& trk     , 
  const std::string&                   key     ,
  const LoKi::Types::TTrCuts&          cuts2tr ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( trk ) 
  , m_track2 ( trk ) 
  , m_same   ( true ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
  , m_cut4rv_trivial ( true  )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from one track source & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& trk     , 
  const std::string&                   key     ,
  const LoKi::Types::TTrCuts&          cuts2tr ,
  const LoKi::Types::RVCuts&           cuts4rv ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( trk ) 
  , m_track2 ( trk ) 
  , m_same   ( true ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )
  , m_sink   ( key )
{}
// ============================================================================
// constructor from one track source & key 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker
( const LoKi::Hlt1::VxMaker::TrSource& trk     , 
  const std::string&                   key     ,
  const LoKi::Types::RVCuts&           cuts4rv ,
  const LoKi::Types::TTrCuts&          cuts2tr )
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_track1 ( trk ) 
  , m_track2 ( trk ) 
  , m_same   ( true ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )
  , m_sink   ( key )
{}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::VxMaker::fillStream ( std::ostream& s ) const 
{
  s << "VxMAKER("           << m_track1 ;
  if ( !m_same ) { s << "," << m_track2 ; }
  s << ",'" << m_sink.selName().str() << "'" ;
  if ( !m_cut2tr_trivial ) { s << "," << m_cut2tr ; }
  if ( !m_cut4rv_trivial ) { s << "," << m_cut4rv ; }  
  return s << ")";
}
// the only one essential method 
// ============================================================================
LoKi::Hlt1::VxMaker::result_type 
LoKi::Hlt1::VxMaker::operator() () const 
{
  typedef TrSource::result_type   Tracks   ;
  typedef LoKi::Combiner_<Tracks> Combiner ;
  typedef Combiner::Range         TRange   ;
  
  result_type output ;
  output.reserve ( 50 ) ;
  
  
  // get the first set of tracks
  Tracks track1 = m_track1() ;
  
  // get the second set of tracks
  Tracks track2 ;
  if ( !m_same ) { track2 = m_track2() ; }
  
  const bool same = m_same || track1 == track2 ;
  
  const TRange range1 ( track1 ) ;
  const TRange range2 ( same ? track1 : track2 ) ;
  
  
  const Hlt::VertexCreator creator ;
  
  /// create the combiner & fill it with data 
  Combiner loop ;
  loop.add ( range1 ) ;
  loop.add ( range2 ) ;
  
  // make the combinations 
  for ( ; loop.valid() ; loop.next() ) 
  {
    // get the current combination 
    Tracks tracks ( 2 ) ;
    loop.current ( tracks.begin() ) ;
    
    const LHCb::Track* first  = tracks[0] ;
    const LHCb::Track* second = tracks[1] ;
    
    // skip invalid 
    if ( 0 == first || 0 == second ) { continue ; }              // CONTINUE 
    // skip the same  
    if ( first == second           ) { continue ; }              // CONTINUE 
    // reduce the double count :
    if ( same && first >= second   ) { continue ; }              // CONTINUE

    // chech the overrlap 
    if ( HltUtils::matchIDs ( *first , *second ) ) { continue ; }// CONTINUE 
    
    // apply the cuts on the combination of two tracks:
    if ( !m_cut2tr_trivial && !m_cut2tr ( *first , *second ) ) { continue ;  }
    
    // create the vertex 
    std::auto_ptr<LHCb::RecVertex> vertex ( new LHCb::RecVertex() ) ;
    
    /// fill it with some information 
    creator ( *first , *second , *vertex ) ;
    
    // apply cuts on the vertex 
    if ( !m_cut4rv_trivial && !m_cut4rv ( *vertex ) ) { continue ; } // CONTINUE
    
    // good vertex! add it to the seelction! 
    output.push_back ( vertex.release() ) ; // "vertex is not valid anymore
    
  }
  
  // use "sink": register object for Hlt Data Service 
  return m_sink ( output )  ; // RETURN 
}



// ============================================================================
// The END 
// ============================================================================
