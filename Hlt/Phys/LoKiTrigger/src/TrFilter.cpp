// $Id$
// ============================================================================
// Include Files 
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrFilter.h"
// ============================================================================
// Local
// ============================================================================
#include "LTTools.h"
// ============================================================================
/** @file
 *  The implementation file for 'upgrade' functors 
 *  @see LoKi:Hlt::FilterTracks 
 * 
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  Version           $Revision$
 *  Last modification $Date$
 *                 by $Author$
 *
 *  @author Roel Aaij roel.aaij@cern.ch
 *  @date 2011-04-08
 */
namespace 
{
  const Gaudi::StringKey s_InfoID{ "InfoID" };
}
// ============================================================================
/*  constructor from configuration parameters 
 *  @param output  the output selection name 
 *  @param tool    the type/name of the underlying tool
 */
// ============================================================================
LoKi::Hlt1::FilterTracks::FilterTracks
( const std::string& output    ,                   // output selection name/key 
  const std::string& toolName  )                   //                 tool name
  : LoKi::AuxFunBase ( std::tie ( output , toolName ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe()
  , m_sink( output   )
  , m_tool( toolName )
{
  // get GaudiAlgorithm 
  GaudiAlgorithm* alg = LoKi::Hlt1::Utils::getGaudiAlg( *this ); 
  Assert( 0 != alg, "GaudiAlgorithm points to NULL!" ); 
  // get IAlgorithm 
  IAlgorithm* ialg = LoKi::Hlt1::Utils::getAlg( *this ); 
  Assert( alg == ialg , "GaudiAlgorithm is not *my* IAlgorithm" ); 
  
  m_alg = alg; 
  
  // retrieve the tool 
  m_selector = m_alg->tool<ITrackSelector>( tool() );
  
  /// get the service 
  SmartIF<IANNSvc> ann = LoKi::Hlt1::Utils::annSvc( *this ); 
  
  auto info = ann->value( s_InfoID, tool() );
  Assert( bool(info) , "request for unknown Info ID :" + tool() );
  
   m_recoID = info->second;
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Hlt1::FilterTracks::~FilterTracks(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::FilterTracks* 
LoKi::Hlt1::FilterTracks::clone() const 
{ return new LoKi::Hlt1::FilterTracks ( *this );  }
// ============================================================================
// OPTIONAL:: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::FilterTracks::fillStream ( std::ostream& s ) const 
{
   return s << "TC_FILTER_TR('" << m_sink.selName() 
            << "',"             << tool() << ")";  
}
// ============================================================================
// MANDATORY : the only essential method 
// ============================================================================
LoKi::Hlt1::FilterTracks::result_type 
LoKi::Hlt1::FilterTracks::operator() 
  ( LoKi::Hlt1::FilterTracks::argument a ) const
{
  result_type output; 
  if ( a.empty() ) {
    // register the selection 
    return !m_sink ? output : m_sink ( output ); 
  }
  
  for( const Hlt::Candidate* candidate: a ) {
    if ( !candidate ) {
      Error ( "Invalid Hlt::Candidate, skip it!");
      continue;
    }
    
    const Hlt::Stage*     stage  = candidate->currentStage(); 
    if ( !stage ) {
         Error ( "Invalid Hlt::Stage,     skip it!");
         continue;
    }
    
    if ( !stage->is<LHCb::Track>() ) {
      Error ( "State does not contain an LHCb::Track, skip it!");
      continue;
    }
    const LHCb::Track* track = stage->get<LHCb::Track>();
    
    // do the filtering
    bool accept = filter( track );
    if ( !accept ) continue;
    
    Hlt::Candidate* _candidate = const_cast< Hlt::Candidate* >( candidate );
    _candidate->addToWorkers( m_alg );
    output.push_back( _candidate );
  }
  // register the selection 
  return !m_sink ? output : m_sink( output );
}
// ============================================================================
bool LoKi::Hlt1::FilterTracks::filter( const LHCb::Track* track ) const
{
  // not reconstructed yet ?
  double info = track->info ( recoID() , -1 );
  if ( -1 == info ) {
    // filter it
    bool accept = m_selector->accept( *track );
    // This is nasty...
    const_cast< LHCb::Track* >( track )->addInfo
      ( recoID(), static_cast< double >( accept ) );
    return accept;
  } else {
    return info;
  }
}
