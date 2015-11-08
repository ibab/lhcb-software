// Include files 

// from std
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/Node.h"
#include "Event/FitNode.h"
#include "Event/VeloCluster.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"

#include "VeloDet/DeVelo.h"

// AIDA
#include "AIDA/IHistogram1D.h"


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
    AIDA::IHistogram1D* overlapNormResidualR ;
    AIDA::IHistogram1D* overlapNormResidualPhi ;
    AIDA::IHistogram1D* residualPhiA ;
    AIDA::IHistogram1D* residualPhiC ;
    AIDA::IHistogram1D* residualRA ;
    AIDA::IHistogram1D* residualRC ;
    
    Histogrammer( const std::string& name,
		  GaudiHistoAlg& parent )
    {
      std::string dirname = name.empty() ? name : name + "/" ;
      overlapResidualR   = parent.book1D(dirname + "overlapResidualR",
					 "R overlap-residual",-0.5, 0.5, 100) ;
      overlapResidualPhi = parent.book1D(dirname + "overlapResidualPhi",
					 "phi overlap-residual", -0.5, 0.5, 100) ;
      overlapNormResidualR   = parent.book1D(dirname + "overlapNormResidualR",
					     "R overlap-residual / pitch",-5, 5, 100) ;
      overlapNormResidualPhi = parent.book1D(dirname + "overlapNormResidualPhi",
					     "phi overlap-residual / pitch", -5, 5, 100) ;
      residualPhiA = parent.book1D(dirname + "residualAPhi","phi residual A side",-0.2,0.2,100) ;
      residualPhiC = parent.book1D(dirname + "residualCPhi","phi residual C side",-0.2,0.2,100) ;
      residualRA = parent.book1D(dirname + "residualAR","R residual A side",-0.2,0.2,100) ;
      residualRC = parent.book1D(dirname + "residualCR","R residual C side",-0.2,0.2,100) ;
    }

    void fillPhi(double residualA, double residualC, double cosalpha, double pitch ) {
      overlapResidualPhi->fill( residualA - residualC ) ;
      residualPhiA->fill( residualA ) ;
      residualPhiC->fill( residualC ) ;
      overlapNormResidualPhi->fill ( (residualA - residualC)/(cosalpha*pitch) ) ;

    }
    void fillR(double residualA, double residualC, double cosalpha, double pitch ) {
      overlapResidualR->fill( residualA - residualC ) ;
      residualRA->fill( residualA ) ;
      residualRC->fill( residualC ) ;
      overlapNormResidualR->fill ( (residualA - residualC)/(cosalpha*pitch) ) ;
    }
  } ;
}

class TrackVeloOverlapMonitor : public GaudiHistoAlg
{

public:
  TrackVeloOverlapMonitor( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrackVeloOverlapMonitor( ); ///< Destructor
  StatusCode execute();    ///< Algorithm execution
  StatusCode initialize() ;
private:
  Histogrammer* histogrammer(int istation, bool top) ;

private:
  std::vector<Histogrammer*> m_histogrammers ;
  Histogrammer* m_sumhistogrammer ;
  std::string m_trackLocation ;
  bool m_omitOutliers ;
  const DeVelo* m_veloDet ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackVeloOverlapMonitor )

TrackVeloOverlapMonitor::TrackVeloOverlapMonitor( const std::string& name,
				ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_histogrammers(64,(Histogrammer*)(0)),
    m_sumhistogrammer(0),
    m_veloDet(0)
{
  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Default  ); 
  declareProperty("OmitOutliers", m_omitOutliers = false) ;
}

TrackVeloOverlapMonitor::~TrackVeloOverlapMonitor() 
{
  for( Histogrammer* histo: m_histogrammers )
    if(histo) delete histo ;
  delete m_sumhistogrammer ;
}

StatusCode TrackVeloOverlapMonitor::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiHistoAlg::initialize();
  m_veloDet = getDet<DeVelo>(  DeVeloLocation::Default ) ;
  return sc ;
}

//=============================================================================
// Main execution
//=============================================================================

namespace {

  bool isASide(const LHCb::Node& node) {
    LHCb::VeloChannelID veloid = node.measurement().lhcbID().veloID() ;
    return veloid.sensor()%2==0 ;
  }

  double computePitch( const LHCb::Node& node ) {
    double pitch = 1 ;
    if( node.measurement().type() == LHCb::Measurement::VeloR ) {
      const LHCb::VeloRMeasurement* meas = 
	dynamic_cast<LHCb::VeloRMeasurement*>(&(node.measurement())) ;
      if(meas)
	pitch = meas->sensor().rPitch( unsigned(meas->cluster()->strip(0)) ) ;
    } else if( node.measurement().type() == LHCb::Measurement::VeloPhi ) {
      const LHCb::VeloPhiMeasurement* meas = 
	dynamic_cast<LHCb::VeloPhiMeasurement*>(&(node.measurement())) ;
      if(meas) {
	const DeVeloPhiType& sensor = meas->sensor() ;
	Gaudi::XYZPoint localPoint = sensor.geometry()->toLocal( node.state().position()) ;
	double r     = localPoint.Rho() ;
	pitch = r * sensor.phiPitch( unsigned(meas->cluster()->strip(0)) ) ;
      }
    }
    return pitch ;
  }

  double computeCosalpha( const LHCb::FitNode& node ) {
    // this vector is along residual, perp to strips
    Gaudi::XYZVector localUnitPocaVector = 
      node.measurement().detectorElement()->geometry()->toLocal( node.pocaVector() ) ;
    return localUnitPocaVector.Rho()/localUnitPocaVector.R() ;
  }
}

Histogrammer* TrackVeloOverlapMonitor::histogrammer(int istation, bool top)
{
  int index = istation ;
  if( top ) index += 22 ;
  if( m_histogrammers[index]==0 ) {
    setHistoTopDir("Track/") ; 
    char name[256] ;
    sprintf(name,"station%d%s",istation, (top ? "Top" : "Bot")) ;
    m_histogrammers[index] = new Histogrammer(std::string(name),*this) ;
  }
  return m_histogrammers[index] ;
}

StatusCode TrackVeloOverlapMonitor::execute() 
{
  // Get tracks.
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_trackLocation);

  if(!m_sumhistogrammer) {
    setHistoTopDir("Track/") ;
    m_sumhistogrammer = new Histogrammer("",*this) ;
  }

  // loop over all tracks
  for( const LHCb::Track* track: tracks) 
    if( track->hasVelo() ) {
      std::vector< std::vector<const LHCb::FitNode*> > phinodes(22) ;
      std::vector< std::vector<const LHCb::FitNode*> > rnodes(22) ;

      for( const LHCb::Node* node: track->nodes() ) 
	if( node->hasMeasurement() &&
	    node->measurement().lhcbID().isVelo() &&
	    (!m_omitOutliers || node->type()==LHCb::Node::HitOnTrack) ) {
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
	    bool top = nodeA->state().y()>0 ;

	    double pitch = computePitch( *nodeA ) ;
	    double cosalpha = computeCosalpha( *nodeA ) ;

	    histogrammer( istation, top )->fillR( residualA, residualC, cosalpha, pitch ) ;
	    m_sumhistogrammer->fillR( residualA, residualC, cosalpha, pitch ) ; 

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
	    bool top = nodeA->state().y()>0 ;
	    
	    double pitch = computePitch( *nodeA ) ;
	    double cosalpha = computeCosalpha( *nodeA ) ;

	    histogrammer( istation, top )->fillPhi( residualA, residualC, cosalpha, pitch ) ;
	    m_sumhistogrammer->fillPhi( residualA, residualC, cosalpha, pitch ) ; 
	    
	  }
	}
      }
    }
  return StatusCode::SUCCESS ;
}
