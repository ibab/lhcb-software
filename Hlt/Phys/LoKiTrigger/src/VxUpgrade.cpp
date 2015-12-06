// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrgToCpp.h"
#include "LoKi/VxUpgrade.h"
// ============================================================================
// local 
// ============================================================================
#include "LTTools.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hlt1::VxUpgrade
 *  @date 2008-11-13 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::VxUpgrade::VxUpgrade 
( const std::string&             output   ,   //      output selection name/key 
  const LoKi::Hlt1::UpgradeConf& config   ,   //                  configuration 
  const LoKi::Hlt1::VxCreator&   creator  ,   //                 vertex creator 
  const std::string&             location )   //  location of upgraded vertices
  : LoKi::AuxFunBase ( std::tie ( output , config , creator , location ) ) 
  , LoKi::BasicFunctors<const LHCb::RecVertex*>::Pipe() 
  , LoKi::Hlt1::UpgradeTool ( config  ) 
  , LoKi::Hlt1::VxCreator   ( creator ) 
  , m_sink     ( output   )  
  , m_location ( location )  
{}
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::VxUpgrade::VxUpgrade 
( const std::string&             output   ,   //      output selection name/key 
  const LoKi::Hlt1::UpgradeConf& config   ,   //                  configuration 
  const std::string&             location ,   //  location of upgraded vertices
  const LoKi::Hlt1::VxCreator&   creator  )   //                 vertex creator 
  : LoKi::AuxFunBase ( std::tie ( output , config , location , creator ) ) 
  , LoKi::BasicFunctors<const LHCb::RecVertex*>::Pipe() 
  , LoKi::Hlt1::UpgradeTool ( config  ) 
  , LoKi::Hlt1::VxCreator   ( creator ) 
  , m_sink     ( output   )  
  , m_location ( location )  
{}
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::VxUpgrade::VxUpgrade 
( const std::string&             output   ,   //      output selection name/key 
  const LoKi::Hlt1::UpgradeTool& config   ,   //                  configuration 
  const std::string&             location ,   // location of upgdarded vertices
  const LoKi::Hlt1::VxCreator&   creator  )   //                 vertex creator 
  : LoKi::AuxFunBase ( std::tie ( output , config , location , creator ) ) 
  , LoKi::BasicFunctors<const LHCb::RecVertex*>::Pipe() 
  , LoKi::Hlt1::UpgradeTool ( config ) 
  , LoKi::Hlt1::VxCreator   ( creator ) 
  , m_sink     ( output   )  
  , m_location ( location )  
{}
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::VxUpgrade::VxUpgrade 
( const std::string&             output   ,   //      output selection name/key 
  const LoKi::Hlt1::UpgradeTool& config   ,   //                  configuration 
  const LoKi::Hlt1::VxCreator&   creator  ,   //                 vertex creator 
  const std::string&             location )   // location of upgdarded vertices
  : LoKi::AuxFunBase ( std::tie ( output , config , creator , location ) ) 
  , LoKi::BasicFunctors<const LHCb::RecVertex*>::Pipe() 
  , LoKi::Hlt1::UpgradeTool ( config ) 
  , LoKi::Hlt1::VxCreator   ( creator ) 
  , m_sink     ( output   )  
  , m_location ( location )  
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Hlt1::VxUpgrade::result_type 
LoKi::Hlt1::VxUpgrade::operator() 
  ( LoKi::Hlt1::VxUpgrade::argument a ) const 
{

  Assert ( alg() , "GaudiAlgorithm* points to NULL!" ) ;
  
  typedef LHCb::RecVertex::Container    Vertices ;
  typedef std::vector<const LHCb::RecVertex*> INPUT    ;
  typedef std::vector<LHCb::RecVertex*> OUTPUT   ;
  typedef std::vector<const LHCb::VertexBase*> SINK_OUTPUT;
  typedef std::vector<const LHCb::RecVertex*> CONST_OUTPUT;
  typedef SmartRefVector<LHCb::Track>   TRKs     ;

  //typedef std::vector<LHCb::Track*>     TRACKS   ;
  typedef LHCb::Track::ConstVector TRACKS   ;
  
  // get container of vertices
  Vertices* vertices = alg() -> getOrCreate<Vertices,Vertices> ( location() ) ;
  
  CONST_OUTPUT output ;
  SINK_OUTPUT sink ;
  // loop over all input vertices 
  for ( const LHCb::RecVertex* vx  :  a )
  {
    if ( !vx ) { Error ( "LHCb::RecVertex* points to NULL, skip it") ; continue ; } 
    
    const TRKs& tracks = vx -> tracks () ;
    if ( 2 != tracks.size() ) { Error ( " nTrack !=  2, skip it!  ") ; continue ; }
    const LHCb::Track* tr1 = tracks[0] ;
    const LHCb::Track* tr2 = tracks[1] ;
    if ( !tr1 || !tr2 ) 
    { Error ( "LHCb::Track* points to NULL, skip it") ; continue ; } 
    
    // upgrade the first track 
    TRACKS tracks1 ;
    StatusCode sc1 = upgrade ( tr1 , tracks1 ) ;
    if ( sc1.isFailure() ) 
    { Error ( "Error from upgrade(track1) , skip " , sc1 ) ; continue ; }
    
    // upgrade the second track 
    TRACKS tracks2 ;
    StatusCode sc2 = upgrade ( tr2 , tracks2 ) ;
    if ( sc2.isFailure() ) 
    { Error ( "Error from upgrade(track2) , skip " , sc2 ) ; continue ; }
    
    if ( tracks1.empty() || tracks2.empty() ) { continue ; }
    
    // create the vertices 
    OUTPUT out ;
    StatusCode sc = make ( tracks1 , tracks2, out ) ;
    if ( sc.isFailure() ) 
    { Warning ("Error from make(), skip " , sc ) ; continue ;  } // CONTINUE
    if ( moveInfo() ) 
    {
      for ( const auto& i : out ) i->setExtraInfo ( vx -> extraInfo () ) ;
    }
    // add vertices into the global list of vertices 
    std::copy( std::begin(out), std::end(out), std::back_inserter(sink) );
    output.insert( output.end(), out.begin(), out.end() );
    // add vertices into the TES constainer  -- this is where the ownership goes!!
    for ( const auto&  i : out ) vertices -> insert ( i ) ; 
    // ========================================================================
  } // end of the loop over all vertices 
  //
  // register vertices for Hlt Data Service
  m_sink( sink );
  return output;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::VxUpgrade::fillStream ( std::ostream& s ) const 
{
  s << "VxUPGRADE("
    << "'" << output () << "'"
    << "," << config () ;
  if ( !trivial() ) 
  { s << ","  ; LoKi::Hlt1::VxCreator::fillStream( s ) ; }
  s << ",'" << location() << "'" ;
  //
  return s << ")" ;  
}

// ============================================================================
// The END 
// ============================================================================
