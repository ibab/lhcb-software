// $Id$
// ============================================================================
// Include files 
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:1572) // non-pointer conversion ... may lose significant bits
#pragma warning(push)
#endif
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/VxCreator.h"
#include "LoKi/Combiner.h"
// ============================================================================
// local 
// ============================================================================
#include "LTTools.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hlt1::VxCreator
 *  @date 2008-11-14 
 *  @author Vanya  BELYAEV Ivan.BElyaev@nikhef.nl
 */
// ============================================================================
// constructor from the cuts 
// ============================================================================
LoKi::Hlt1::VxCreator::VxCreator
( const LoKi::Types::TrCuts&  cuts    , 
  const LoKi::Types::TTrCuts& cuts2tr ,
  const LoKi::Types::RVCuts&  cuts4rv ) 
  : LoKi::AuxFunBase () 
  //
  , m_cut    ( cuts    ) 
  , m_cut_trivial    ( false ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )  
{}
// ============================================================================
// constructor from the cuts 
// ============================================================================
LoKi::Hlt1::VxCreator::VxCreator
( const LoKi::Types::TTrCuts& cuts2tr ,
  const LoKi::Types::RVCuts&  cuts4rv ) 
  : LoKi::AuxFunBase () 
  //
  , m_cut    ( LoKi::Constant<const LHCb::Track*,bool>( true ) ) 
  , m_cut_trivial    ( true  ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )  
{}
// ============================================================================
// constructor from the cuts 
// ============================================================================
LoKi::Hlt1::VxCreator::VxCreator
( const LoKi::Types::TrCuts&  cuts    ,
  const LoKi::Types::RVCuts&  cuts4rv ) 
  : LoKi::AuxFunBase () 
  //
  , m_cut    ( cuts    ) 
  , m_cut_trivial    ( false ) 
  , m_cut2tr ( LoKi::Constant<LoKi::TrackTypes::TrackPair,bool>( true ) ) 
  , m_cut2tr_trivial ( true ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )  
{}
// ============================================================================
// constructor from the cuts 
// ============================================================================
LoKi::Hlt1::VxCreator::VxCreator
( const LoKi::Types::TrCuts&  cuts    ,
  const LoKi::Types::TTrCuts& cuts2tr ) 
  : LoKi::AuxFunBase () 
  //
  , m_cut    ( cuts    ) 
  , m_cut_trivial    ( false ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
  , m_cut4rv_trivial ( true )
{}
// ============================================================================
// constructor from the cuts 
// ============================================================================
LoKi::Hlt1::VxCreator::VxCreator
( const LoKi::Types::TrCuts& cuts ) 
  : LoKi::AuxFunBase () 
  //
  , m_cut    ( cuts    ) 
  , m_cut_trivial    ( false ) 
  , m_cut2tr ( LoKi::Constant<LoKi::TrackTypes::TrackPair,bool>( true ) ) 
  , m_cut2tr_trivial ( true ) 
  , m_cut4rv ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
  , m_cut4rv_trivial ( true )
{}
// ============================================================================
// constructor from the cuts 
// ============================================================================
LoKi::Hlt1::VxCreator::VxCreator
( const LoKi::Types::TTrCuts& cuts2tr ) 
  : LoKi::AuxFunBase () 
//
  , m_cut    ( LoKi::Constant<const LHCb::Track*,bool>( true ) ) 
  , m_cut_trivial    ( true  ) 
  , m_cut2tr ( cuts2tr ) 
  , m_cut2tr_trivial ( false ) 
  , m_cut4rv ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
  , m_cut4rv_trivial ( true )
{}
// ============================================================================
// constructor from the cuts 
// ============================================================================
LoKi::Hlt1::VxCreator::VxCreator
( const LoKi::Types::RVCuts&  cuts4rv ) 
  : LoKi::AuxFunBase () 
  //
  , m_cut    ( LoKi::Constant<const LHCb::Track*,bool>( true ) ) 
  , m_cut_trivial    ( true  ) 
  , m_cut2tr ( LoKi::Constant<LoKi::TrackTypes::TrackPair,bool>( true ) ) 
  , m_cut2tr_trivial ( true ) 
  , m_cut4rv ( cuts4rv ) 
  , m_cut4rv_trivial ( false )  
{}
// ============================================================================
// constructor from the cuts 
// ============================================================================
LoKi::Hlt1::VxCreator::VxCreator () 
  : LoKi::AuxFunBase () 
  //
  , m_cut    ( LoKi::Constant<const LHCb::Track*,bool>( true ) ) 
  , m_cut_trivial    ( true  ) 
  , m_cut2tr ( LoKi::Constant<LoKi::TrackTypes::TrackPair,bool>( true ) ) 
  , m_cut2tr_trivial ( true ) 
  , m_cut4rv ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
  , m_cut4rv_trivial ( true )
{}
// ============================================================================
// nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::VxCreator::fillStream ( std::ostream& s ) const 
{
  s << "LoKi.Hlt1.VxCreator(" ;
  //
  typedef std::vector<std::string> Strings ;
  Strings out ;
  //
  if ( !m_cut_trivial    ) { out.push_back( cuts    () . printOut () ) ; }
  if ( !m_cut2tr_trivial ) { out.push_back( cuts2tr () . printOut () ) ; }
  if ( !m_cut4rv_trivial ) { out.push_back( cuts4rv () . printOut () ) ; }
  //
  for ( Strings::const_iterator iout = out.begin() ; out.end() != iout ; ++iout ) 
  {
    if ( out.begin () !=iout ) { s << "," ; }
    s << ( *iout ) ;
  }
  //
  return s << ")" ;
}
// ============================================================================
// make the vertices
// ============================================================================
size_t LoKi::Hlt1::VxCreator::make 
( const LHCb::Track::ConstVector&  tracks1  , 
  const LHCb::Track::ConstVector&  tracks2  , 
  std::vector<LHCb::RecVertex*>&   vertices ) const 
{
  if ( tracks1.empty() || tracks2.empty() ) { return 0 ; }
  
  const bool same = 
    ( &tracks1 == &tracks2 ) || ( tracks1 == tracks2 ) ;
  
  const size_t n1 = tracks1.size() ;
  const size_t n2 = tracks2.size() ;
  
  vertices.reserve ( same ? 
                     vertices.size() + n1*(n1-1)/2 : 
                     vertices.size() + n1*n2       ) ;
  
  typedef LHCb::Track::ConstVector   Tracks   ;
  typedef LoKi::Combiner_<Tracks>    Combiner ;
  
  const Hlt::VertexCreator           creator    ;
  const LoKi::Hlt1::Utils::CmpTrack  compare = LoKi::Hlt1::Utils::CmpTrack()  ;
  
  const size_t size = vertices.size() ;
  
  /// create the combiner & fill it with data 
  Combiner loop ;
  loop.add ( { tracks1 }  ) ;
  loop.add ( { tracks2 }  ) ;
  
  // make the combinations 
  for ( ; loop.valid() ; loop.next() ) 
  {
    // get the current combination 
    Tracks tracks ( 2 ) ;
    loop.current ( tracks.begin() ) ;
    
    const LHCb::Track* first  = tracks[0] ;
    const LHCb::Track* second = tracks[1] ;
    
    // skip invalid 
    if ( 0 == first || 0 == second      ) { continue ; }              // CONTINUE 
    // skip the same  
    if ( first == second                ) { continue ; }              // CONTINUE 
    // reduce the double count :
    if ( same && !compare ( first , second ) ) { continue ; }         // CONTINUE
    
    // chech the overrlap 
    if ( HltUtils::matchIDs ( *first , *second ) ) { continue ; }     // CONTINUE 
    
    // apply the cuts on the combination of two tracks:
    if ( !m_cut2tr_trivial && !m_cut2tr ( *first , *second ) ) { continue ;  }
    
    // create the vertex 
    std::auto_ptr<LHCb::RecVertex> vertex ( new LHCb::RecVertex() ) ;
    
    /// fill it with some information 
    creator ( *first , *second , *vertex ) ;
    
    // apply cuts on the vertex 
    if ( !m_cut4rv_trivial && !m_cut4rv ( *vertex ) ) { continue ; } // CONTINUE
    
    // good vertex! add it to the outptu container 
    vertices.push_back ( vertex.release() ) ; // "vertex is not valid anymore
    
  }  // end of loop over all combinations  
  //
  // return number of created vertcies 
  return vertices.size() - size ; // RETURN 
}
// ============================================================================
// The END 
// ============================================================================
