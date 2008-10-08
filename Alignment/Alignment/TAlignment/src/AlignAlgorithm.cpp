// $Id: AlignAlgorithm.cpp,v 1.52 2008-10-08 11:50:33 wouter Exp $
// Include files
// from std
// #include <utility>
#include <algorithm>
#include <iterator>
#include <fstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiUtils/Aida2ROOT.h"

// from root
#include "TH1D.h"

// from DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/GeometryInfoPlus.h"
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/GlobalToLocalDelta.h"
#include "DetDesc/Condition.h"
#include "DetDesc/AlignmentCondition.h"

// from Kernel
#include "Kernel/LHCbID.h"

// from BOOST
#include "boost/assign/std/vector.hpp"
#include "boost/assign/list_of.hpp"
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

// local
#include "AlignAlgorithm.h"
#include "AlParameters.h"
#include "SolvKernel/AlEquations.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignAlgorithm
//
// 2007-03-05 : Jan Amoraal
//-----------------------------------------------------------------------------

namespace {

 Gaudi::Vector6 convert(const Gaudi::Matrix1x6& m) {
   Gaudi::Vector6 v;
   for (int i = 0; i < 6; ++i) v(i) = m(0,i);
   return v;
 }

 class Data {
 public:
   Data(unsigned nodeindex, unsigned index, double r, const Gaudi::Matrix1x6& d,bool isoutlier)
     : m_nodeindex(nodeindex),
       m_index(index),
       m_r(r),
       m_d(d),
       m_isOutlier(isoutlier) {}
   unsigned index() const { return m_index; }
   double r() const { return m_r; }
   const Gaudi::Matrix1x6& d() const { return m_d; }
   size_t nodeindex() const { return m_nodeindex ; }
   bool isOutlier() const { return m_isOutlier ; }
 private:
   size_t            m_nodeindex;
   unsigned          m_index;
   double            m_r;
   Gaudi::Matrix1x6  m_d;
   bool              m_isOutlier ;
 };
 
};

using namespace LHCb;
using namespace boost;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AlignAlgorithm );

AlignAlgorithm::AlignAlgorithm( const std::string& name,
				ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_iteration(0u),
    m_nIterations(0u),
    m_nTracks(0u),
    m_covFailure(0u),
    m_elements(),
    m_align(0),
    m_projSelector(0),
    m_trackresidualtool("Al::TrackResidualTool"),
    m_vertexresidualtool("Al::VertexResidualTool"),
    m_updatetool("Al::AlignUpdateTool"),
    m_equations(0),
    m_resetHistos(false)
{
  declareProperty("NumberOfIterations"          , m_nIterations                  = 10u                     );
  declareProperty("TracksLocation"              , m_tracksLocation               = TrackLocation::Default  );
  declareProperty("VertexLocation"              , m_vertexLocation               = ""  );
  declareProperty("ProjectorSelector"           , m_projSelectorName             = "TrackProjectorSelector");
  declareProperty("UseCorrelations"             , m_correlation                  = true                    );
  declareProperty("UpdateInFinalize"            , m_updateInFinalize             = false                   );
  declareProperty("OutputDataFile"              , m_outputDataFileName           = "alignderivatives.dat" ) ;
  declareProperty("InputDataFiles"              , m_inputDataFileNames ) ;
  declareProperty("Chi2Outlier"                 , m_chi2Outlier                  = 10000 ) ;
  declareProperty("UpdateTool",m_updatetool) ;
  declareProperty("TrackResidualTool",m_trackresidualtool) ;
  declareProperty("VertexResidualTool",m_vertexresidualtool) ;
}

AlignAlgorithm::~AlignAlgorithm() {}


StatusCode AlignAlgorithm::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  /// Set histogram path
    //if ("" == histoTopDir()) setHistoTopDir("Alignment/");

  /// Get pointer to incident service and add listener
  IIncidentSvc* incSvc = svc<IIncidentSvc>("IncidentSvc");
  if (!incSvc) return Error("==> Failed to retrieve IncidentSvc!", StatusCode::FAILURE);
  incSvc->addListener(this, "UpdateConstants");

  /// Get tool to align detector
  m_align = tool<IGetElementsToBeAligned>("GetElementsToBeAligned");
  if (!m_align) return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);

  sc = m_trackresidualtool.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_vertexresidualtool.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_updatetool.retrieve() ;
  if ( sc.isFailure() ) return sc;

  /// Get range  detector elements
  m_elements = m_align->rangeElements();
  
  if (printDebug()) {
    debug() << "==> Got " << m_elements.size() << " elements to align!" << endmsg;
    for(ElementRange::const_iterator i = m_elements.begin(); i!= m_elements.end(); ++i) {
      const AlParameters::DofMask& ownDoFMask = i->dofMask();
      debug() <<  "        " << (*i) << endmsg;
      const std::vector<std::string> dofs = boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz");
      debug() << "DOFs: ";
      for (AlParameters::DofMask::const_iterator j = ownDoFMask.begin(), jEnd = ownDoFMask.end(); j != jEnd; ++j) {
        if ((*j)) info() << dofs.at(std::distance(ownDoFMask.begin(), j));
      }
      debug() << endmsg;
    }
  }
  
  m_equations = new Al::Equations(m_elements.size());
  m_equations->clear() ;
  assert( m_elements.size() == m_equations->nElem() ) ;
  for(std::vector<std::string>::const_iterator ifile = m_inputDataFileNames.begin() ; 
      ifile != m_inputDataFileNames.end(); ++ifile) {
    Al::Equations tmp(m_equations->nElem()) ;
    tmp.readFromFile( (*ifile).c_str() ) ;
    m_equations->add( tmp ) ; 
    warning() << "Adding derivatives from input file: " << *ifile << " " << tmp.numHits() << " "
	      << tmp.totalChiSquare() << " " << m_equations->totalChiSquare() << endreq ;
  }

  /// Get projector selector tool
  m_projSelector = tool<ITrackProjectorSelector>(m_projSelectorName, "Projector", this);
  if (!m_projSelector) return Error("==> Failed to retrieve projector selector tool", StatusCode::FAILURE);

  /// Monitoring
  /// Book residual histograms
  /// Residuals
  /// @todo: this should go into a monitoring tool
  info() << "booking histograms assuming " << m_nIterations << " iterations " << endmsg;
  m_trackChi2Histo           = book2D(10, "Track chi2 distribution vs iteration",
				      -0.5, m_nIterations-0.5, m_nIterations, -1.00, 
                                      +100.00, 100);
  m_trackNorChi2Histo        = book2D(11, "Normalised track chi2 distribution vs iteration", 
                                      -0.5, m_nIterations-0.5, m_nIterations,0,20) ;
  for(ElementRange::const_iterator i = m_elements.begin(); i!= m_elements.end(); ++i) 
    m_elemHistos.push_back( new AlElementHistos(*this,*i,m_nIterations) ) ;
  m_resetHistos = false ;

  return StatusCode::SUCCESS;
}

StatusCode AlignAlgorithm::finalize() {
  if(!m_outputDataFileName.empty()) 
    m_equations->writeToFile( m_outputDataFileName.c_str() ) ;
  if (m_updateInFinalize) update() ;

  for( std::vector<AlElementHistos*>::iterator ielem = m_elemHistos.begin() ;
       ielem != m_elemHistos.end(); ++ielem) delete *ielem ;

  m_trackresidualtool.release() ;
  m_vertexresidualtool.release() ;
  m_updatetool.release() ;
  return GaudiHistoAlg::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignAlgorithm::execute() {

  // Reset histograms if required
  if(m_resetHistos) resetHistos() ;

  // Get tracks. Copy them into a vector, because that's what we need for dealing with vertices.
  const Tracks* tracks = get<Tracks>(m_tracksLocation);
  if (printVerbose()) verbose() << "Number of tracks in container " + m_tracksLocation + ": " << tracks->size() << endmsg;
  m_nTracks += tracks->size() ;

  // First just copy the tracks
  std::vector<const LHCb::Track*> selectedtracks ;
  for( Tracks::const_iterator iTrack = tracks->begin(), iTrackEnd = tracks->end(); iTrack != iTrackEnd; ++iTrack) 
    // just a sanity check
    if( (*iTrack)->fitStatus()==LHCb::Track::Fitted &&
	(*iTrack)->nDoF() > 0 &&
	!(*iTrack)->nodes().empty() ) {
      const Al::Residuals* res = m_trackresidualtool->get(**iTrack) ;
      if( res ) {
	selectedtracks.push_back( *iTrack ) ;
      } else {
	warning() << "Error computing residual cov matrix. Skipping track of type "
		  << (*iTrack)->type() << " with key: " << (*iTrack)->key() << " and chi2 / dof: " << (*iTrack)->chi2() << "/" << (*iTrack)->nDoF() << endmsg ;
	++m_covFailure;
      }
    } else {
      warning() << "Skipping bad track:"
		<< " fitstatus = " << (*iTrack)->fitStatus()
		<< " nDoF = " << (*iTrack)->nDoF()
		<< " #nodes = " << (*iTrack)->nodes().size() << endreq ;
    }
  
  // Now I got a bit worried about overlaps. Sort these tracks in the
  // number of LHCbIDs. Then remove overlapping tracks.
  std::sort(selectedtracks.begin(), selectedtracks.end(), 
	    boost::lambda::bind(&LHCb::Track::nLHCbIDs,*boost::lambda::_1) > 
	    boost::lambda::bind(&LHCb::Track::nLHCbIDs,*boost::lambda::_2)) ;
  std::vector< const LHCb::Track* > nonoverlappingtracks ;
  std::set< unsigned int > selectedlhcbids ;
  for( std::vector<const LHCb::Track*>::const_iterator itr = selectedtracks.begin() ;
       itr != selectedtracks.end(); ++itr) {
    std::set<unsigned int> ids ;
    for( std::vector<LHCb::LHCbID>::const_iterator iid = (*itr)->lhcbIDs().begin() ;
	 iid != (*itr)->lhcbIDs().end(); ++iid) ids.insert( iid->lhcbID() ) ;
    //std::set<LHCb::LHCbID> ids( (*itr)->lhcbIDs().begin(), (*itr)->lhcbIDs().end() ) ;
    if( ids.size() != (*itr)->lhcbIDs().size() ) {
      warning() << "Skipping track with non-unique LHCbIds" << endreq ;
    } else {
      std::set<unsigned int> allids = selectedlhcbids ;
      allids.insert( ids.begin(), ids.end() ) ;
      if( allids.size() != selectedlhcbids.size() + ids.size() ) {
	//warning() << "Skipping track of type " << (*itr)->type() 
	// << "because it overlaps with another selected track" << endreq ;
      } else {
	nonoverlappingtracks.push_back(*itr) ;
	selectedlhcbids = allids ;
      }
    }
  }
  if( selectedtracks.size() != nonoverlappingtracks.size() ) 
    warning() << "Rejected " << selectedtracks.size() - nonoverlappingtracks.size() << " out of "
	      << selectedtracks.size() << " tracks because of overlaps." << endreq ;

  selectedtracks = nonoverlappingtracks ;
  

  // Now deal with vertices, if there are any.
  size_t numusedtracks(0) ;
  size_t numusedvertices(0) ;
  if( !m_vertexLocation.empty() ) {
    const LHCb::RecVertices* vertices = get<LHCb::RecVertices>(m_vertexLocation);
    if(vertices ) {
      for( LHCb::RecVertices::const_iterator ivertex = vertices->begin() ;
	   ivertex != vertices->end(); ++ivertex ) {
	// used tracks are automatically removed from the output
	// list. if the vertex is not accepted (e.g. because there are
	// not enough tracks), then a 0 pointer is returned.
	const Al::MultiTrackResiduals* res = m_vertexresidualtool->get(**ivertex,selectedtracks) ;
	if (res && accumulate( *res ) ) {
	  m_equations->addVertexChi2Summary( res->vertexChi2(), res->vertexNDoF() ) ;
	  // need some histogramming here
	  ++numusedvertices ;
	  numusedtracks += res->numTracks() ;
	} 
      }
    }
  }	
  
  // Loop over the remaining tracks
  if (printVerbose()) verbose() << "Number of tracks left after processing vertices: " << selectedtracks.size() << endreq ;
  for( std::vector<const LHCb::Track*>::const_iterator iTrack = selectedtracks.begin() ;
       iTrack != selectedtracks.end(); ++iTrack ) {
    
    // this cannot return a zero pointer since we have already checked before
    const Al::Residuals* res = m_trackresidualtool->get(**iTrack) ;
    assert(res!=0) ;
    if( res && accumulate( *res ) ) {
      m_trackChi2Histo->fill(m_iteration, (*iTrack)->chi2());
      m_trackNorChi2Histo->fill(m_iteration, (*iTrack)->chi2PerDoF());
      ++numusedtracks ;
    }
  } 
  
  m_equations->addEventSummary( numusedtracks, numusedvertices ) ;
  
  return StatusCode::SUCCESS;
}


bool AlignAlgorithm::accumulate( const Al::Residuals& residuals )
{
  if (printVerbose()) verbose() << "==> Found " << residuals.nodes().size() << " nodes"<< endmsg;
  std::vector<Data> data;
  // Loop over nodes and get measurements, residuals and errors
  typedef Al::Residuals::NodeContainer::const_iterator NodeIter;
  size_t numexternalhits(0) ;
  size_t nodeindex(0) ;
  std::set<unsigned> alignables ;
  for (Al::Residuals::NodeContainer::const_iterator node = residuals.nodes().begin(); 
       node != residuals.nodes().end();++node, ++nodeindex) {
    assert( (*node)->hasMeasurement() ) ;

    // Get measurement
    const Measurement& meas = (*node)->measurement();
    // Get element that belongs to this measurment
    const AlignmentElement* elem = m_align->findElement(meas);
    if (!elem) {
      if (printVerbose()) verbose() << "==> Measurement not on a to-be-aligned DetElem " 
				    << meas.lhcbID() << " "
				    << meas.detectorElement()->name() << endmsg;
      ++numexternalhits ;
      continue;
    }
      
    const unsigned index = elem->index();
    if (printVerbose()) verbose() << "==> measure = " << meas.measure() << " id = " << meas.lhcbID() << " -> index = " 
				  << index << " -> " <<  elem->name() << endmsg;
    
    // Project measurement
    const ITrackProjector* proj = m_projSelector->projector(meas);
    if (!proj) {
      error() << "==> Could not get projector for selected measurement!" << endmsg;
      continue;
    }
    double res    = residuals.r(nodeindex) ;
    double err    = std::sqrt(residuals.V(nodeindex)) ;
    // Get alignment derivatives
    LHCb::StateVector state((*node)->state().stateVector(),(*node)->state().z());
    // the projector calculates the derivatives in the global
    // frame. The jacobian corrects for the transformation to the
    // alignment frame.
    Derivatives der =  
      proj->alignmentDerivatives(state,meas,Gaudi::XYZPoint(0,0,0)) *
      elem->jacobian() ;
    
    bool isoutlier = res*res/(*node)->errResidual2() > m_chi2Outlier ;
    
    // push back normalized residuals & derivatives;
    res /= err;
    der /= err;
    data.push_back(Data(nodeindex, index, res, der, isoutlier));
    alignables.insert(index) ;
  }

  // reject this track if there is only a single alignable element and no external hits
  bool accept = !data.empty() && ( alignables.size()>=2 || numexternalhits >= 1 ) ;
  if ( accept ) {
    
    for (std::vector<Data>::const_iterator id = data.begin(), idEnd = data.end(); id != idEnd; ++id) {
      m_equations->addHitSummary(id->index(), residuals.V(id->nodeindex()), residuals.R(id->nodeindex())) ;
      // outliers are not added to the first derivative. they must be
      // added to the 2nd though, because otherwise we loose the
      // correlations. we'll solve the relative normalization when we
      // build the full linear system.
      if( !id->isOutlier() ) 
	m_equations->V(id->index())            -= convert(id->r()*id->d()) ;
      else
	m_equations->numOutliers(id->index())  += 1 ;
      
      m_equations->M(id->index(), id->index()) += (Transpose(id->d())*id->d());
      
      for (std::vector<Data>::const_iterator jd = data.begin(); jd != idEnd; ++jd) {
	if ( id == jd || ( m_correlation && id->index() <= jd->index() )) {
	  double c = residuals.HCH_norm( id->nodeindex(), jd->nodeindex() ) ;
	  m_equations->M(id->index(), jd->index()) -= c * (Transpose(id->d())*jd->d());
	}
      }
          
      // compute the derivative of the curvature, used for one of the
      // canonical constraints:
      //   dx/dalpha = - dchi^2/dalphadx * ( dchi^2/dx^2)^{-1}
      //             = - 2 * dr/dalpha * V^{-1} * H * C
      // This stil neds seom work because I catually want the
      // derivative to the first state.

      const Gaudi::TrackSymMatrix& C = residuals.nodes()[id->nodeindex()]->state().covariance() ;
      const Gaudi::TrackProjectionMatrix& H = residuals.nodes()[id->nodeindex()]->projectionMatrix() ;
      const ROOT::Math::SMatrix<double,5,1> normalizeddrdstate = C*Transpose(H) / std::sqrt(residuals.V(id->nodeindex())) ;
      m_equations->dStateDAlpha(id->index()) += normalizeddrdstate * id->d() ;
      
      // fill some histograms
      {
	double V = residuals.V(id->nodeindex()) ;
	double R = residuals.R(id->nodeindex()) ;
	double r = residuals.r(id->nodeindex()) ;
	double f = std::sqrt(R/V) ;
	double pull = r / sqrt(R) ;
	
	size_t index = id->index() ;
	double sign = id->d()(0,0) > 0 ? 1 : -1 ;
	m_elemHistos[index]->m_nHitsHisto->fill(m_iteration);
	m_elemHistos[index]->m_resHisto->fill(m_iteration, sign*r);
	m_elemHistos[index]->m_pullHisto->fill(m_iteration, sign*pull);
	m_elemHistos[index]->m_autoCorrHisto->fill(m_iteration,f) ;
	for(int ipar=0; ipar<6; ++ipar) {
	  double weight = id->d()(0,ipar) * f ;
	  double thispull = pull ;
	  if(weight<0) { weight *= -1 ; thispull *= -1 ; }
	  // the last minus sign is because somebody defined our first derivative with the wrong sign
	  m_elemHistos[index]->m_residualPullHistos[ipar]->fill( -thispull, weight ) ;
	}
      } 
    } 
    // keep some information about the tracks that we have seen
    m_equations->addChi2Summary( residuals.chi2(), residuals.nDoF(), numexternalhits ) ;
  }
  return accept ;
}
 
 
//=============================================================================
//  Update
//=============================================================================
void AlignAlgorithm::update() 
{
  if(m_equations) {
    info() << "Total number of tracks: " << m_nTracks << endreq ;
    info() << "Number of covariance calculation failures: " << m_covFailure << endreq ;
    update(*m_equations) ;
  }
}

void AlignAlgorithm::update(const Al::Equations& equations)
{
  m_updatetool->process(equations,m_iteration,m_nIterations).ignore() ;
}

void AlignAlgorithm::reset() {
  if (printDebug()) debug() << "increasing iteration counter and resetting accumulated data..." << endmsg;
  /// increment iteration counter
  ++m_iteration;
  // set counters to zero
  m_nTracks = 0u;
  m_covFailure = 0u;
  // clear derivatives and H maps
  m_equations->clear();
  m_resetHistos = true ;
}

void AlignAlgorithm::resetHistos() 
{
  m_resetHistos = false ;
  // moved this seperately such that histograms are not reset on last iteration
  std::for_each(m_elemHistos.begin(),m_elemHistos.end(),
		boost::lambda::bind(&AlElementHistos::reset,*boost::lambda::_1)) ;
}

StatusCode AlignAlgorithm::queryInterface(const InterfaceID& id, void** ppI) {
  // check the placeholder:
  if (0 == ppI) return StatusCode::FAILURE;
  // check ID of the interface
  if (IIncidentListener::interfaceID() != id) return Algorithm::queryInterface(id, ppI);
  *ppI = static_cast<IIncidentListener*>(this);
  addRef();
  return StatusCode::SUCCESS;
}

void AlignAlgorithm::handle(const Incident& incident) {
  if ("UpdateConstants" == incident.type()) {
    update();
    reset();
  }
}
