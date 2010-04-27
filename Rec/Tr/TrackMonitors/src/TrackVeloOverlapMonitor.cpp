// $Id: TrackVeloOverlapMonitor.cpp,v 1.2 2010-04-27 07:38:21 wouter Exp $
// Include files 


// Include files

// from std
#include <string>
#include <vector>
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/AlgFactory.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/Node.h"
#include "Event/FitNode.h"

// AIDA
#include "AIDA/IHistogram1D.h"

#include <boost/foreach.hpp>

/** @class AlignAlgorithm AlignAlgorithm.h
*
*
*  @author Wouter Hulsbergen
*  @date   2008-09-07
*/

namespace {
  struct Histogrammer
  {
    AIDA::IHistogram1D* overlapResidualR ;
    AIDA::IHistogram1D* overlapResidualPhi ;
    AIDA::IHistogram1D* residualPhiA ;
    AIDA::IHistogram1D* residualPhiC ;
    AIDA::IHistogram1D* residualRA ;
    AIDA::IHistogram1D* residualRC ;
    
    Histogrammer( int istation,
		  GaudiHistoAlg& parent ) 
    {
      char dirname[256] ;
      sprintf(dirname,"station%d/",istation) ;
      std::string station(dirname) ;
      overlapResidualR   = parent.book1D(station + "overlapResidualR",
					 "R overlap-residual",-0.5, 0.5, 100) ;
      overlapResidualPhi = parent.book1D(station + "overlapResidualPhi",
					 "phi overlap-residual", -0.5, 0.5, 100) ;
      residualPhiA = parent.book1D(station + "residualPhiA","phi residual A side",-0.2,0.2,100) ;
      residualPhiC = parent.book1D(station + "residualPhiC","phi residual C side",-0.2,0.2,100) ;
      residualRA = parent.book1D(station + "residualRA","R residual A side",-0.2,0.2,100) ;
      residualRC = parent.book1D(station + "residualRC","R residual C side",-0.2,0.2,100) ;
    }
  } ;
}

class TrackVeloOverlapMonitor : public GaudiHistoAlg
{

public:
  TrackVeloOverlapMonitor( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrackVeloOverlapMonitor( ); ///< Destructor
  virtual StatusCode execute();    ///< Algorithm execution

private:
  Histogrammer* histogrammer(int istation) ;

private:
  std::vector<Histogrammer*> m_histogrammers ;
  std::string m_trackLocation ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackVeloOverlapMonitor );

TrackVeloOverlapMonitor::TrackVeloOverlapMonitor( const std::string& name,
				ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_histogrammers(22,(Histogrammer*)(0))
{
  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Default  ); 
}

TrackVeloOverlapMonitor::~TrackVeloOverlapMonitor() 
{
  BOOST_FOREACH( Histogrammer* histo, m_histogrammers )
    if(histo) delete histo ;
}

//=============================================================================
// Main execution
//=============================================================================

namespace {
  bool isASide(const LHCb::Node& node) {
    LHCb::VeloChannelID veloid = node.measurement().lhcbID().veloID() ;
    return veloid.sensor()%2==0 ;
  }
}

Histogrammer* TrackVeloOverlapMonitor::histogrammer(int istation)
{
  if( m_histogrammers[istation]==0 ) {
    setHistoTopDir("Track/") ;
    m_histogrammers[istation] = new Histogrammer(istation,*this) ;
  }
  return m_histogrammers[istation] ;
}

StatusCode TrackVeloOverlapMonitor::execute() 
{
  // Get tracks.
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_trackLocation);

  // loop over all tracks
  BOOST_FOREACH( const LHCb::Track* track, tracks) 
    if( track->hasVelo() ) {
      std::vector< std::vector<const LHCb::FitNode*> > phinodes(22) ;
      std::vector< std::vector<const LHCb::FitNode*> > rnodes(22) ;

      BOOST_FOREACH( const LHCb::Node* node, track->nodes() ) 
	if( node->hasMeasurement() &&
	    node->measurement().lhcbID().isVelo() ) {
	  LHCb::VeloChannelID veloid = node->measurement().lhcbID().veloID() ;
	  unsigned int station = (veloid.sensor()%64)/2 ; 
	  //unsigned int side    = (veloid.sensor()%2) ; 
	  unsigned int type    = (veloid.sensor()/64) ;
	  // ignore pile-up
	  const LHCb::FitNode* fitnode = dynamic_cast<const LHCb::FitNode*>(node) ;
	  if( type==0 ) 
	    rnodes[station].push_back(fitnode) ;
	  else if( type==1)
	    phinodes[station].push_back(fitnode) ; 
	}
	
      for(size_t istation=0; istation<22; ++istation) {
	if( rnodes[istation].size() == 2 ) {
	  const LHCb::FitNode* nodeA = rnodes[istation].front() ;
	  const LHCb::FitNode* nodeC = rnodes[istation].back() ;
	  if( isASide(*nodeC) ) std::swap(nodeA,nodeC) ;
	  if( isASide(*nodeA) && !isASide(*nodeC)  ) { // ameks ure we actaully have both sides :-(
	    double residualA = nodeA->unbiasedResidual() ;
	    if(nodeA->projectionMatrix()(0,1) < 0 ) residualA *= -1 ;
	    double residualC = nodeC->unbiasedResidual() ;
	    if(nodeC->projectionMatrix()(0,1) < 0 ) residualC *= -1 ;
	    
	    Histogrammer* hg = histogrammer( istation ) ;
	    hg->overlapResidualR->fill( residualA - residualC ) ;
	    hg->residualRA->fill( residualA ) ;
	    hg->residualRC->fill( residualC ) ;
	  }
	}
	if( phinodes[istation].size() == 2 ) { 
          const LHCb::FitNode* nodeA = phinodes[istation].front() ; 
          const LHCb::FitNode* nodeC = phinodes[istation].back() ; 
          if( isASide(*nodeC) ) std::swap(nodeA,nodeC) ;
	  if( isASide(*nodeA) && !isASide(*nodeC)  ) { // ameks ure we actaully have both sides :-(
	    double residualA = nodeA->unbiasedResidual() ; 
	    if(nodeA->projectionMatrix()(0,0) < 0 ) residualA *= -1 ; 
	    double residualC = nodeC->unbiasedResidual() ; 
	    if(nodeC->projectionMatrix()(0,0) < 0 ) residualC *= -1 ; 
	    Histogrammer* hg = histogrammer( istation ) ; 
	    hg->overlapResidualPhi->fill( residualA - residualC ) ; 
	    hg->residualPhiA->fill( residualA ) ; 
	    hg->residualPhiC->fill( residualC ) ; 
	  } 
	}
      }
    }
  return StatusCode::SUCCESS ;
}
