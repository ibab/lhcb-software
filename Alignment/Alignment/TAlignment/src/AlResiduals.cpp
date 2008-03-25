#include "AlResiduals.h"
#include "Event/Node.h"

namespace Al
{
  
  size_t Residuals::index( const LHCb::Node& node) const 
  {
    size_t rc(0) ;
    while(  rc != m_nodes.size() && m_nodes[rc] != &node ) ++rc ;
    assert( rc < m_nodes.size() ) ;
    //if( rc == m_nodes.size() ) throw std::runtime_error ;
    return rc ;
  }

  Residuals::Residuals( const NodeContainer& nodes, double chisq, size_t ndof ) 
    : m_nodes(nodes),
      m_r(nodes.size()),
      m_V(nodes.size()),
      m_HCH(nodes.size(),0),
      m_chi2(chisq),
      m_nDoF(ndof)
  {
    for(size_t index = 0 ; index != nodes.size(); ++index ) {
      const LHCb::Node* node = nodes[index] ;
      m_r(index+1) = node->residual() ;
      m_V(index+1) = node->errMeasure2() ;
      m_HCH(index+1,index+1) = node->errMeasure2() - node->errResidual2() ;
    }
  }

  TrackResiduals::TrackResiduals(const LHCb::Track& track,
				 const NodeContainer& nodes) 
    : Residuals(nodes, track.chi2(), track.nDoF()),
      m_state( nodes.front()->state() ),
      m_residualStateCov(size()) 
  {
  }
}
