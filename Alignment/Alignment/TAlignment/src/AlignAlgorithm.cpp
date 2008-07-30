// $Id: AlignAlgorithm.cpp,v 1.49 2008-07-30 12:43:07 wouter Exp $
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
 
 //FIXME: the next two stand-alone functions should move into AlVec & AlSymMat...
 template <typename T>
 void ass(AlVec& lhs, unsigned row, const T& rhs) {
   for (unsigned i = 0; i < T::kSize; ++i) lhs[row+i] = rhs(i);
 }
 
 template <typename T>
 void ass(AlSymMat& lhs, unsigned col, unsigned row, const T& rhs) {
   for (unsigned i = 0; i < T::kCols; ++i)
     for (unsigned j = 0; j < T::kRows; ++j)
       lhs[col+i][row+j] = rhs(i,j);
 }
 
 void fillHisto1D(IHistogram1D* histo1D, const size_t& bin, const double& value, const double& error) {
   /// Get pointer to root histo
   TH1D* rHisto1D = Gaudi::Utils::Aida2ROOT::aida2root(histo1D);
   rHisto1D->SetBinContent(bin, value); rHisto1D->SetBinError(bin, error);
 }

  class Constraints
  {
  public:
    enum EConstraints { Tx, Ty, Tz, Rx, Ry, Rz, Szx, Szy, Szz, Cur, NumConstraints } ;
    static std::vector<std::string> all()  { return names ; }
    static std::vector<std::string> global()  { 
      return std::vector<std::string>( names.begin(), names.begin()+6) ;
    }
    static std::string name(size_t i)  { return names[i] ; }
    static size_t index(const std::string& aname) {
      std::vector<std::string>::const_iterator it = std::find( names.begin(), names.end(), aname) ;
      return size_t( it - names.begin() ) ;
      //return std::distance( names.begin(),it) ; // doesn't exist for random access iterator ?!
    }
  private:
    static const std::vector<std::string> names ;
  } ;
  
  const std::vector<std::string> Constraints::names = 
    boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz")("Szx")("Szy")("Szz")("Cur");
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
    m_matrixSolverTool(0),
    m_trackresidualtool("Al::TrackResidualTool"),
    m_vertexresidualtool("Al::VertexResidualTool"),
    m_constrainttool("Al::AlignConstraintTool"),
    m_equations(0),
    m_resetHistos(false)
{
  declareProperty("NumberOfIterations"          , m_nIterations                  = 10u                     );
  declareProperty("TracksLocation"              , m_tracksLocation               = TrackLocation::Default  );
  declareProperty("VertexLocation"              , m_vertexLocation               = ""  );
  declareProperty("ProjectorSelector"           , m_projSelectorName             = "TrackProjectorSelector");
  declareProperty("MatrixSolverTool"            , m_matrixSolverToolName         = "SpmInvTool"            );
  declareProperty("UseCorrelations"             , m_correlation                  = true                    );
  declareProperty("UpdateInFinalize"            , m_updateInFinalize             = false                   );
  declareProperty("MinNumberOfHits"             , m_minNumberOfHits              = 100u                    ); 
  declareProperty("UsePreconditioning"          , m_usePreconditioning           = false                   );
  declareProperty("OutputDataFile"              , m_outputDataFileName           = "alignderivatives.dat" ) ;
  declareProperty("InputDataFiles"              , m_inputDataFileNames ) ;
  declareProperty("LogFile"                     , m_logFileName                  = "alignlog.txt" ) ;
  declareProperty("Chi2Outlier"                 , m_chi2Outlier                  = 10000 ) ;
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
  m_align = tool<IGetElementsToBeAligned>("GetElementsToBeAligned", this);
  if (!m_align) return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);

  sc = m_trackresidualtool.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_vertexresidualtool.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_constrainttool.retrieve() ;
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

  ///Get matrix solver tool
  m_matrixSolverTool = tool<IAlignSolvTool>(m_matrixSolverToolName, "MatrixSolver", this);
  if (!m_matrixSolverTool) return Error("==> Failed to retrieve matrix solver tool", StatusCode::FAILURE);

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
  m_totNusedTracksvsIterHisto= bookProfile1D(20, "Total number of used tracks for alignment vs iteration" , -0.5, m_nIterations-0.5, m_nIterations);
  m_totChi2vsIterHisto       = bookProfile1D(30, "Total sum of track chi2 vs iteration"            , -0.5, m_nIterations-0.5, m_nIterations);
  m_avgChi2vsIterHisto       = bookProfile1D(31, "Average sum of track chi2 vs iteration"          , -0.5, m_nIterations-0.5, m_nIterations);
  m_norTotChi2vsIterHisto    = bookProfile1D(32, "Normalised total sum of track chi2 vs iteration" , -0.5, m_nIterations-0.5, m_nIterations);
  m_dAlignChi2vsIterHisto    = bookProfile1D(40, "Delta Alignment chi2 vs iteration"               , -0.5, m_nIterations-0.5, m_nIterations);
  m_nordAlignChi2vsIterHisto = bookProfile1D(41, "Delta Alignment normalised chi2 vs iteration"    , -0.5, m_nIterations-0.5, m_nIterations);
  
  for(ElementRange::const_iterator i = m_elements.begin(); i!= m_elements.end(); ++i) 
    m_elemHistos.push_back( new AlElementHistos(*this,*i,m_nIterations) ) ;
  m_resetHistos = false ;

  return StatusCode::SUCCESS;
}

StatusCode AlignAlgorithm::finalize() {
  if(!m_outputDataFileName.empty()) 
    m_equations->writeToFile( m_outputDataFileName.c_str() ) ;
  if (m_updateInFinalize) update() ;
  if(!m_logFileName.empty()) {
    std::ofstream logfile(m_logFileName.c_str()) ;
    logfile << m_logMessage.str() << std::endl ;
  }

  for( std::vector<AlElementHistos*>::iterator ielem = m_elemHistos.begin() ;
       ielem != m_elemHistos.end(); ++ielem) delete *ielem ;

  m_trackresidualtool.release() ;
  m_vertexresidualtool.release() ;
  m_constrainttool.release() ;
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
 
 
void AlignAlgorithm::preCondition(AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2,
				  AlVec& scale) const
{  
  // This is just not sufficiently fool proof!
  size_t size = halfDChi2DAlpha.size() ;
  scale.reSize(size) ;
  for (size_t i = 0u, iEnd = size; i< iEnd; ++i) scale[i] = 1 ;
  int iElem(0u) ;
  for (ElementRange::const_iterator it = m_elements.begin(), itEnd = m_elements.end(); it != itEnd; ++it, ++iElem) {
    int offset = it->activeParOffset() ;
    if ( 0<=offset ) {
      size_t ndof = (*it).dofMask().nActive() ;
      size_t N = m_equations->numHits(iElem) ;
      for (size_t i = offset; i< offset+ndof; ++i) {
        assert( i < size ) ;
        if ( halfD2Chi2DAlpha2[i][i] > 0 ) scale[i] = std::sqrt( N / halfD2Chi2DAlpha2[i][i] ) ;
      }
    }
  }
  
  for (size_t i = 0u, iEnd = size; i < iEnd; ++i) {
    halfDChi2DAlpha[i] *= scale[i] ;
    for (size_t j = 0u; j <= i; ++j) halfD2Chi2DAlpha2[i][j] *= scale[i] * scale[j] ;
  }
}

void AlignAlgorithm::postCondition(AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2,
				   const AlVec& scale) const
{ 
  size_t size = halfDChi2DAlpha.size() ;
  for (size_t i = 0u, iEnd = size; i < iEnd; ++i) {
    halfDChi2DAlpha[i] *= scale[i] ;
    for (size_t j = 0u; j<=i; ++j) halfD2Chi2DAlpha2[i][j] *= scale[i] * scale[j] ;
  }
}

//=============================================================================
//  Update
//=============================================================================

void AlignAlgorithm::update() {

  if ( m_equations->nElem() == 0 ) {
    warning() << "==> No elements to align." << endmsg ;
    return ;
  }
  
  info() << "\n";
  info() << "==> iteration " << m_iteration << " : Initial alignment conditions  : [";
  std::vector<double> deltas;
  deltas.reserve(m_elements.size()*6u);  
  getAlignmentConstants(m_elements, deltas);
  for (std::vector<double>::const_iterator i = deltas.begin(), iEnd = deltas.end(); i != iEnd; ++i) {
    info() << (*i) << (i != iEnd-1u ? ", " : "]");
  }
  info() << endmsg;

  info() << "==> Updating constants" << endmsg ;
  std::ostringstream logmessage ;
  logmessage << "********************* ALIGNMENT LOG ************************" << std::endl
	     << "Iteration: " << m_iteration << std::endl
	     << "Total number of events: " << m_equations->numEvents() << std::endl 
	     << "Total number of tracks: " << m_nTracks << std::endl
	     << "Number of covariance calculation failures: " << m_covFailure << std::endl
	     << "Used " << m_equations->numVertices() << " vertices for alignment" << std::endl
             << "Used " << m_equations->numTracks() << " tracks for alignment" << std::endl
	     << "Total chisquare/dofs: " << m_equations->totalChiSquare() << " / " << m_equations->totalNumDofs() << std::endl
	     << "Average track chisquare: " << m_equations->totalTrackChiSquare() / m_equations->numTracks() << std::endl
	     << "Track chisquare/dof: " << m_equations->totalTrackChiSquare() / m_equations->totalTrackNumDofs() << std::endl
	     << "Vertex chisquare/dof: " 
	     << (m_equations->totalVertexNumDofs()>0 ? m_equations->totalVertexChiSquare() / m_equations->totalVertexNumDofs() : 0.0 ) << std::endl
	     << "Used " << m_equations->numHits() << " hits for alignment" << std::endl
             << "Total number of hits in external detectors: " << m_equations->numExternalHits() << std::endl;

  m_totNusedTracksvsIterHisto->fill(m_iteration, m_equations->numTracks());
  m_totChi2vsIterHisto->fill(m_iteration, m_equations->totalChiSquare());
  m_avgChi2vsIterHisto->fill(m_iteration, m_equations->totalChiSquare() / m_equations->numTracks());
  m_norTotChi2vsIterHisto->fill(m_iteration, m_equations->totalChiSquare() / m_equations->totalNumDofs());

  if (printDebug()) {
    for (size_t i = 0; i < m_equations->nElem(); ++i) {
      for (size_t j = i; j < m_equations->nElem(); ++j) {
	debug() << "==> M["<<i<<","<<j<<"] = "      << m_equations->M(i,j) << endmsg;
      }
      debug() << "\n==> V["<<i<<"] = "    << m_equations->V(i) << endmsg;
    }
  }

  // Create the dof mask and a map from AlignableElements to an
  // offset. The offsets are initialized with '-1', which signals 'not
  // enough hits'.
  size_t numParameters(0), numExcluded(0) ;
  for (ElementRange::const_iterator it = m_elements.begin(); it != m_elements.end(); ++it ) {
    if (m_equations->numHits(it->index()) >= m_minNumberOfHits) {
      it->setActiveParOffset( numParameters ) ;
      numParameters += it->dofMask().nActive() ;
    } else {
      it->setActiveParOffset(-1) ;
      ++numExcluded ;
    }
  }
  
  if(numParameters>0) {

    // now fill the 'big' vector and matrix
    AlVec    halfDChi2dX(numParameters) ;
    AlSymMat halfD2Chi2dX2(numParameters);
    size_t index(0);
    int iactive, jactive ;
    for( Al::Equations::ElementContainer::const_iterator ieq = m_equations->elements().begin() ;
	 ieq != m_equations->elements().end(); ++ieq, ++index ) {
      const AlignmentElement& ielem = m_elements[index] ;
      // first derivative. note that we correct here for the fraction of outliers
      double outliercorrection = 1./ieq->fracNonOutlier() ;
      for(unsigned ipar=0; ipar<Derivatives::kCols; ++ipar) 
	if( 0<= (iactive = ielem.activeParIndex(ipar) ) )
	  halfDChi2dX(iactive) = ieq->V()(ipar) * outliercorrection ;
      
      // second derivative. fill only non-zero terms
      for( Al::ElementData::OffdiagonalContainer::const_iterator im = ieq->M().begin() ;
	   im != ieq->M().end(); ++im ) {
	size_t jndex = im->first ;
	// guaranteed: index <= jndex .. that's how we fill it. furthermore, row==i, col==j
	if(jndex==index) { // diagonal element
	  for(unsigned ipar=0; ipar<Derivatives::kCols; ++ipar) 
	    if( 0<= ( iactive = ielem.activeParIndex(ipar) ) ) 
	      for(unsigned jpar=0; jpar<=ipar; ++jpar) 
		if( 0<= ( jactive = ielem.activeParIndex(jpar) ) )
		  halfD2Chi2dX2.fast(iactive,jactive) = im->second(ipar,jpar) ;
	} else {
	  const AlignmentElement& jelem = m_elements[jndex] ;
	  for(unsigned ipar=0; ipar<Derivatives::kCols; ++ipar) 
	    if( 0<= ( iactive = ielem.activeParIndex(ipar) ) ) 
	      for(unsigned jpar=0; jpar<Derivatives::kCols; ++jpar) 
		if( 0<= ( jactive = jelem.activeParIndex(jpar) ) ) {
		  assert(jactive > iactive  ) ;
		  halfD2Chi2dX2.fast(jactive,iactive) = im->second(ipar,jpar) ;
		}
	}
      }
    }
    // add the constraints
    size_t numConstraints = m_constrainttool->addConstraints(m_elements,*m_equations,halfDChi2dX,halfD2Chi2dX2) ;
    
    logmessage << "Number of alignables with insufficient statistics: " << numExcluded << std::endl
	       << "Number of constraints: "                             << numConstraints << std::endl
	       << "Number of active parameters: "                       << numParameters << std::endl ;
    
    int numDoFs = halfDChi2dX.size() ;
    if (numDoFs < 50 ) {
      info() << "AlVec Vector    = " << halfDChi2dX << endmsg;
      info() << "AlSymMat Matrix = " << halfD2Chi2dX2      << endmsg;
    } else {
      info() << "Matrices too big to dump to std" << endmsg ;
    }
    
    // Tool returns H^-1 and alignment constants. Need to copy the 2nd derivative because we still need it.
    AlVec    scale(halfD2Chi2dX2.size()) ;
    AlSymMat covmatrix = halfD2Chi2dX2 ;
    AlVec    solution  = halfDChi2dX ;
    if (m_usePreconditioning) preCondition(solution,covmatrix, scale) ;
    bool solved = m_matrixSolverTool->compute(covmatrix, solution);
    if (m_usePreconditioning) postCondition(solution,covmatrix, scale) ;
    
    if (solved) {
      double deltaChi2 = solution * halfDChi2dX ; //somewhere we have been a bit sloppy with two minus signs!
      if (printDebug()) {
	info() << "Solution = " << solution << endmsg ;
	info() << "Covariance = " << covmatrix << endmsg ;
      }
      logmessage << "Alignment change chisquare/dof: " 
		 << deltaChi2 << " / " << numParameters << std::endl
                 << "Normalised alignment change chisquare: " << deltaChi2 / numParameters << std::endl;
      
      m_dAlignChi2vsIterHisto->fill(m_iteration, deltaChi2) ;
      m_nordAlignChi2vsIterHisto->fill(m_iteration, deltaChi2 /numParameters);
      
      m_constrainttool->printConstraints(solution, covmatrix, logmessage) ;
      
      if (printDebug()) debug() << "==> Putting alignment constants" << endmsg;
      size_t iElem(0u) ;
      double totalLocalDeltaChi2(0) ; // another figure of merit of the size of the misalignment.
      for (ElementRange::iterator it = m_elements.begin(); it != m_elements.end(); ++it, ++iElem) {
	logmessage << "Alignable: " << it->name() << std::endl
		   << "Number of hits/outliers seen: " << m_equations->numHits(iElem) << " "
		   << m_equations->numOutliers(iElem) << std::endl ;
	int offset = it->activeParOffset() ;
	if( offset < 0 ) {
	  logmessage << "Not enough hits for alignment. Skipping update." << std::endl ;
	} else {
	  AlParameters delta( solution, covmatrix, halfD2Chi2dX2, it->dofMask(), offset ) ;
	  AlParameters refdelta = it->currentActiveDelta() ;
	  //logmessage << delta ;
	  for(unsigned int iactive = 0u; iactive < delta.dim(); ++iactive) 
	    logmessage << std::setw(6)  << delta.activeParName(iactive) << ":" 
		       << " cur= " << std::setw(12) << refdelta.parameters()[iactive]
		       << " delta= " << std::setw(12) << delta.parameters()[iactive] << " +/- "
		       << std::setw(12) << AlParameters::signedSqrt(delta.covariance()[iactive][iactive]) 
		       << " gcc= " << delta.globalCorrelationCoefficient(iactive) << std::endl ;
	  double contributionToCoordinateError = delta.measurementCoordinateSigma( m_equations->weightR(iElem) ) ;
	  double coordinateError = std::sqrt(m_equations->numHits(iElem)/m_equations->weight(iElem)) ;
	  logmessage << "contribution to hit error (absolute/relative): "
		     << contributionToCoordinateError << " " << contributionToCoordinateError/coordinateError << std::endl ;
	  
	  // compute another figure of merit for the change in
	  // alignment parameters that does not rely on the
	  // correlations. this should also go into either
	  // AlParameters or AlEquation
	  const Gaudi::Matrix6x6& thisdChi2dAlpha2 = m_equations->elements()[iElem].M().find(iElem)->second ;
	  Gaudi::Vector6 thisAlpha = delta.parameterVector6() ;
	  double thisLocalDeltaChi2 = ROOT::Math::Dot(thisAlpha,thisdChi2dAlpha2 * thisAlpha) ;
	  logmessage << "local delta chi2 / dof: " << thisLocalDeltaChi2 << " / " << delta.dim() << std::endl ;
	  totalLocalDeltaChi2 += thisLocalDeltaChi2 ;
	  //double d2 = m_equations->elements()[iElem].M()[iElem]


	  // need const_cast because loki range givess access only to const values 
	  StatusCode sc = (const_cast<AlignmentElement&>(*it)).updateGeometry(delta) ;
	  if (!sc.isSuccess()) error() << "Failed to set alignment condition for " << it->name() << endmsg ; 
          fillHisto1D(m_elemHistos[it->index()]->m_deltaTxHisto, m_iteration+1u, delta.translation()[0], delta.errTranslation()[0]);
          fillHisto1D(m_elemHistos[it->index()]->m_deltaTyHisto, m_iteration+1u, delta.translation()[1], delta.errTranslation()[1]);
          fillHisto1D(m_elemHistos[it->index()]->m_deltaTzHisto, m_iteration+1u, delta.translation()[2], delta.errTranslation()[2]);
          fillHisto1D(m_elemHistos[it->index()]->m_deltaRxHisto, m_iteration+1u, delta.rotation()[0]   , delta.errRotation()[0]);
          fillHisto1D(m_elemHistos[it->index()]->m_deltaRyHisto, m_iteration+1u, delta.rotation()[1]   , delta.errRotation()[1]);
          fillHisto1D(m_elemHistos[it->index()]->m_deltaRzHisto, m_iteration+1u, delta.rotation()[2]   , delta.errRotation()[2]);
        }
      }
      logmessage << "total local delta chi2 / dof: " << totalLocalDeltaChi2 << " / " << numParameters << std::endl ;
    } else {
      error() << "Failed to solve system" << endmsg ;
    }
  } else {
    logmessage << "No alignable degrees of freedom ... skipping solver tools and update." << std::endl ;
  }
  
  logmessage << "********************* END OF ALIGNMENT LOG ************************" ;
  info() << logmessage.str() << endmsg ;
  info() << "\n";
  info() << "==> iteration " << m_iteration << " : Updated alignment conditions  : [";
  deltas.clear();
  getAlignmentConstants(m_elements, deltas);
  for (std::vector<double>::const_iterator i = deltas.begin(), iEnd = deltas.end(); i != iEnd; ++i) {
    info() << (*i) << (i != iEnd-1u ? ", " : "]");
  }
  info() << endmsg;
  m_logMessage << logmessage.str() ;
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

void AlignAlgorithm::getAlignmentConstants(const ElementRange& rangeElements, AlignConstants& alignConstants) const {
  alignConstants.reserve(6*rangeElements.size()); /// 6 constants * num elements
  for (ElementRange::const_iterator i = rangeElements.begin(); i != rangeElements.end(); ++i) {
    if (printDebug()) debug() << "Getting alignment constants for " << (*i) << endmsg;
    /// Get translations and rotations
    const std::vector<double>& translations = i->deltaTranslations();
    const std::vector<double>& rotations    = i->deltaRotations();
    /// Insert intitial constants (per element)
    alignConstants.insert(alignConstants.end(), translations.begin(), translations.end());
    alignConstants.insert(alignConstants.end(), rotations.begin()   , rotations.end()   );
  }
}
