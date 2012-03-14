// ============================================================================
// Include files
// ============================================================================
#include <boost/foreach.hpp>
// ============================================================================
// Local
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
#include "LoKi/AuxFunBase.h"
#include "LTTools.h"
#include "LoKi/TrackCutAsRecVertexCut.h"
// ============================================================================
/* @file
 *
 * Implementation file for the classes Hlt_TrackCutAsRecVertexCut_Any and
 * Hlt_TrackCutAsRecVertexCut_All
 *
 * @date 2012-02-27
 * @author Pieter David pieter.david@cern.ch
 */

void LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_Any::retrieveFailKey()
{
  // from LoKi::Hlt1::HelperTool
  // get GaudiAlgorithm
  GaudiAlgorithm*  alg = LoKi::Hlt1::Utils::getGaudiAlg ( *this ) ;
  Assert ( 0 != alg    , "GaudiAlgorithm points to NULL!" ) ;
  // get IAlgorithm
  IAlgorithm*     ialg = LoKi::Hlt1::Utils::getAlg      ( *this ) ;
  Assert ( alg == ialg , "GaudiAlgorithm is not *my* IAlgorithm" ) ;
  // get the InfoID from HltANNSvc
  SmartIF<IANNSvc> ann = LoKi::Hlt1::Utils::annSvc( *this ) ;
  const std::string infoIDName = "HltUnit/" + alg->name() ;
  boost::optional<IANNSvc::minor_value_type> _info = ann->value("InfoID" , infoIDName );
  Assert( _info , " request for unknown Info ID : " + infoIDName );
  m_extraInfoKey = _info->second;

}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_Any::result_type
LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_Any::operator()
  ( LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_Any::argument v ) const
{
  result_type ret = false;
  BOOST_FOREACH( SmartRef<LHCb::Track> tr, v->tracks() )
  {
    if ( ! m_useExtraInfo ) {
      ret = ret || ( m_cut( tr ) );
    } else {
      if ( ! tr->hasInfo( m_extraInfoKey ) ) {
        result_type pass = m_cut( tr );
        ret = ret || pass;
        if ( ! pass ) {
          tr->addInfo( m_extraInfoKey, 1. );
        }
      }
    }
  }
  return ret;
}

// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_All::result_type
LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_All::operator()
  ( LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_All::argument v ) const
{
  result_type ret = true;
  SmartRefVector<LHCb::Track>::const_iterator it = v->tracks().begin();
  while ( ret && ( it != v->tracks().end() ) ) {
    ret = m_cut( *it );
    ++it;
  }
  return ret;
}

// ============================================================================
// The END
// ============================================================================
