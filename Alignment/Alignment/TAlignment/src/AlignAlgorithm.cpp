// $Id: AlignAlgorithm.cpp,v 1.18 2008-01-29 16:35:31 wouter Exp $
// Include files
// from std
// #include <utility>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDetDataSvc.h"

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

// local
#include "AlignAlgorithm.h"
#include "ResidualCovarianceTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignAlgorithm
//
// 2007-03-05 : Jan Amoraal
//-----------------------------------------------------------------------------

namespace {

  class Equations {
  public:
    Equations(size_t nElem) 
      : m_nElem(nElem), 
        m_v(nElem),
	m_numHits(nElem,0), m_numTracks(0), m_totalChiSquare(0), m_totalNumDofs(0), m_numExternalHits(0)
    {}
    void clear() ;
    size_t nElem() const { return m_nElem; }
    Gaudi::Vector6&       V(int i) { return m_v[i]; }
    const Gaudi::Vector6& V(int i) const { return m_v[i]; }
    Gaudi::Matrix6x6& M(int i, int j) { assert(i<=j); return m_m[std::make_pair(i,j)]; }
    //FIXME: return proxy that fixes i<j...
    const Gaudi::Matrix6x6& M(int i, int j) const { 
      assert(i<=j); 
      return const_cast<Equations*>(this)->m_m[std::make_pair(i,j)]; 
    }
    size_t& numHits(int i) { return m_numHits[i] ; }
    size_t numHits(int i) const { return m_numHits[i] ; }
    
    void addTrackSummary( double chisq, size_t ndof, size_t nexternal) { 
      m_totalChiSquare +=chisq; 
      m_totalNumDofs   +=ndof ; 
      m_numExternalHits+=nexternal ;
      ++m_numTracks; 
    }
    size_t numTracks() const { return m_numTracks ; }
    double totalChiSquare() const { return m_totalChiSquare ; }
    size_t totalNumDofs() const { return m_totalNumDofs ; }
    size_t numExternalHits() const { return m_numExternalHits ; }

  private:
    size_t                                         m_nElem ;
    std::vector<Gaudi::Vector6>                    m_v;
    std::map<std::pair<int,int>, Gaudi::Matrix6x6> m_m;
    std::vector<size_t>                            m_numHits ;
    size_t                                         m_numTracks ;
    double                                         m_totalChiSquare ;
    size_t                                         m_totalNumDofs ;
    size_t                                         m_numExternalHits ;
  };
  
  void Equations::clear() 
  {
    m_v.clear(); m_v.resize(m_nElem); 
    m_m.clear(); 
    m_numHits.clear() ; m_numHits.resize(m_nElem,0) ;
    m_totalChiSquare = 0 ;
    m_numTracks = 0 ;
    m_totalNumDofs = 0 ;
    m_numExternalHits = 0 ;
  }

  Gaudi::Vector6 convert(const Gaudi::Matrix1x6& m) {
    Gaudi::Vector6 v;
    for (int i = 0; i < 6; ++i) v(i) = m(0,i);
    return v;
  };
  
  
  class Data {
  public:
    Data(const LHCb::Node& node, unsigned index, double r, const Gaudi::Matrix1x6& d)
      : m_id(&node),
        m_index(index),
        m_r(r),
        m_d(d) {}
    const LHCb::Node* id() const { return m_id ; }
    unsigned index() const { return m_index; }
    double r() const { return m_r; }
    const Gaudi::Matrix1x6& d() const { return m_d; }
  private:
    const LHCb::Node* m_id ;
    unsigned         m_index;
    double           m_r;
    Gaudi::Matrix1x6 m_d;
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
				  m_rangeElements(),
				  m_nDoFs(0u),
				  m_dofMask(),
				  m_initAlignConstants(),
				  m_align(0),
				  m_projSelector(0),
				  m_matrixSolverTool(0),
				  m_equations(0),
				  m_constraints()
{
  declareProperty("NumberOfIterations"   , m_nIterations          = 10u                     );
  declareProperty("TracksLocation"       , m_tracksLocation       = TrackLocation::Default  );
  declareProperty("ProjectorSelector"    , m_projSelectorName     = "TrackProjectorSelector");
  declareProperty("MatrixSolverTool"     , m_matrixSolverToolName = "SpmInvTool"            );
  declareProperty("UseCorrelations"      , m_correlation          = true                    );
  declareProperty("UpdateInFinalize"     , m_updateInFinalize     = false                   );
}

AlignAlgorithm::~AlignAlgorithm() {}


StatusCode AlignAlgorithm::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  /// Set histogram path
  if ("" == histoTopDir()) setHistoTopDir("Alignment/");

  /// Get pointer to incident service and add listener
  IIncidentSvc* incSvc = svc<IIncidentSvc>("IncidentSvc");
  if (!incSvc) return Error("==> Failed to retrieve IncidentSvc!", StatusCode::FAILURE);
  incSvc->addListener(this, "UpdateConstants");

  /// Get tool to align detector
  m_align = tool<IGetElementsToBeAligned>("GetElementsToBeAligned", this);
  if (!m_align) return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);

  /// Get range  detector elements
  m_rangeElements = m_align->rangeElements();

  if (printDebug())
    debug() << "==> Got " << std::distance(m_rangeElements.first, m_rangeElements.second) << " elements to align!" << endmsg;
    
  for (ElemIter i = m_rangeElements.first, iEnd = m_rangeElements.second; i != iEnd; ++i) {
    m_nDoFs += i->nDOFs();
    const std::vector<bool>& ownDoFMask = i->dofMask();
    if (printDebug()) {
      debug() <<  "        " << (*i) << endmsg;
      const std::vector<std::string> dofs = boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz");
      debug() << "DOFs: ";
      for (std::vector<bool>::const_iterator j = ownDoFMask.begin(), jEnd = ownDoFMask.end(); j != jEnd; ++j) {
        if ((*j)) info() << dofs.at(std::distance(ownDoFMask.begin(), j));
      }
      debug() << endmsg;
    }
    m_dofMask.insert(m_dofMask.end(), ownDoFMask.begin(), ownDoFMask.end());
  }
  
  m_equations = new Equations(std::distance(m_rangeElements.first, m_rangeElements.second));
  m_equations->clear() ;

  if (printDebug()) {
    debug() << "nDOFs             = " << m_nDoFs          << endmsg;
    debug() << "dofsflags         = " << m_dofMask        << endmsg;
    debug() << "size of dofsflags = " << m_dofMask.size() << endmsg;
  }
  
  // get constraints
  m_constraints = m_align->constraints();
  if (printDebug()) {
    debug() << "Number of constraints = " << m_constraints.size() << endmsg;
    unsigned nC = 0u;
    for (std::vector<std::vector<double> >::const_iterator i = m_constraints.begin(), iEnd = m_constraints.end(); i != iEnd;
         ++i, ++nC) {
      debug() << nC << ": f = [ ";
      for (std::vector<double>::const_iterator j = i->begin(), jEnd = i->end()-1; j != jEnd; ++j) {
        info()  << (*j) << " ";
      }
      debug() << " ]" << endmsg;
      debug() << "   f0 = " << i->back() << endmsg;
    }
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
  for (ElemIter i = m_rangeElements.first, iEnd = m_rangeElements.second; i != iEnd; ++i) {
    const unsigned    indexI = i->index();
    const std::string name   = i->name();
    if (printDebug()) debug() << "Booking histograms for element " << name << " with index " << indexI <<endmsg;
    m_resHistos[indexI]     = book2D(1000u+indexI, "residual vs iteration for " + name,
                                     -0.5, m_nIterations-0.5, m_nIterations,
                                     -1.00 , +1.00, 100);
    m_pullHistos[indexI]     = book2D(2000u+indexI, "pull vs iteration for " + name,
                                      -0.5, m_nIterations-0.5, m_nIterations,
                                      -5.00 , +5.00, 100);
    m_nhitHistos[indexI]     = book1D(3000u+indexI, "number of hits vs iteration for " + name,
                                      -0.5, m_nIterations-0.5, m_nIterations);
    m_autoCorrHistos[indexI] = book2D(4000u+indexI, "hit autocorrelation in " + name + "  vs iteration ",
                                      -0.5, m_nIterations-0.5, m_nIterations,
                                      -0.5,+0.5,250);
    unsigned(indexJ) = indexI;
    for (ElemIter j = i; j != iEnd; ++j, ++indexJ) {
      m_corrHistos[std::make_pair(indexI, indexJ)] = book2D((10000 + indexI) * (100 + indexJ),
                                                            "hit correlation of " + name + " with " + j->name()
                                                            + " vs iteration "
                                                            + ( i == j ? "(excluding autocorrelations!)" : "" ),
                                                            -0.5, m_nIterations-0.5, m_nIterations,
                                                            -1.0, +1.0, 250);
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode AlignAlgorithm::finalize()
{
  if(m_updateInFinalize) update() ;
  return GaudiHistoAlg::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignAlgorithm::execute() {

  // Get tracks
  Tracks* tracks = get<Tracks>(m_tracksLocation);
  if (printDebug()) debug() << "Number of tracks in container " + m_tracksLocation + ": " << tracks->size() << endmsg;
  /// Loop over tracks
  typedef Tracks::const_iterator TrackIter;
  for (TrackIter iTrack = tracks->begin(), iTrackEnd = tracks->end(); iTrack != iTrackEnd; ++iTrack) {
    //
    std::vector<Data> data;
    // Get nodes. Need them for measurements, residuals and errors
    const Nodes& nodes = (*iTrack)->nodes();
    if (printDebug()) debug() << "==> Found " << nodes.size() << " nodes"<< endmsg;

    // Loop over nodes and get measurements, residuals and errors
    typedef Nodes::const_iterator NodeIter;
    size_t numexternalhits(0) ;
    for (NodeIter node = nodes.begin(), end = nodes.end(); node != end; ++node) 
      if ((*node)->hasMeasurement()) {
	// Get measurement
	const Measurement& meas = (*node)->measurement();
	// Get element that belongs to this measurment
	const AlignmentElement* elem = m_align->findElement(meas.lhcbID());
	if (!elem) {
	  if (printDebug()) debug() << "==> Measurement not on a to-be-aligned DetElem " 
				    << meas.lhcbID() << endmsg;
	  ++numexternalhits ;
	  continue;
	}
	const unsigned index = elem->index();
	if (printDebug()) debug() << "==> measure = " << meas.measure() << " id = " << meas.lhcbID() << " -> index = " 
				  << index << " -> " <<  elem->name() << endmsg;
	// Project measurement
	ITrackProjector* proj = m_projSelector->projector(meas);
	if (!proj) {
	  error() << "==> Could not get projector for selected measurement!" << endmsg;
	  continue;
	}
	double res  = (*node)->residual();
	double err  = (*node)->errMeasure();
	
	m_nhitHistos[index]->fill(m_iteration);
	m_resHistos[index]->fill(m_iteration, res);
	m_pullHistos[index]->fill(m_iteration, res/err);
	// Get alignment derivatives
	LHCb::StateVector state((*node)->state().stateVector(),(*node)->state().z());
	Derivatives der = proj->alignmentDerivatives(state, meas, elem->pivotXYZPoint());
	// push back normalized residuals & derivatives;
	res /= err;
	der /= err;
	data.push_back(Data(**node, index, res, der));
      }
      
    if (!data.empty()) {
      ResidualCovarianceTool cov;
      cov.compute(*(*iTrack));
      
      for (std::vector<Data>::const_iterator id = data.begin(), idEnd = data.end(); id != idEnd; ++id) {
	m_equations->numHits(id->index())         += 1 ;
	m_equations->V(id->index())               -= convert(id->r()*id->d()) ;
	m_equations->M(id->index(), id->index())  += (Transpose(id->d())*id->d());
	
	for (std::vector<Data>::const_iterator jd = data.begin(); jd != idEnd; ++jd) 
	  if( id == jd || ( m_correlation && id->index() <= jd->index() )) {
	    double c = cov.HCH_norm(*id->id(),*jd->id());
	    m_equations->M(id->index(), jd->index()) -= c * (Transpose(id->d())*jd->d());
	    
	    if (!( id->id() == jd->id())) {
	      m_corrHistos[std::make_pair(id->index(), jd->index())]->
		fill(m_iteration, c/std::sqrt(cov.HCH_norm(*id->id(), *id->id())*cov.HCH_norm(*jd->id(), *jd->id())));
	    } else {
	      m_autoCorrHistos[id->index()]->fill(m_iteration, c);
	    }
	  }
      } 
      
      // keep some information about the tracks that we have seen
      m_equations->addTrackSummary( (*iTrack)->chi2(), (*iTrack)->nDoF(), numexternalhits ) ;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Update
//=============================================================================
void AlignAlgorithm::update() {

  info() << "==> Updating constants" << std::endl
         << "==> Used " << m_equations->numTracks() << " tracks for alignment" << std::endl
	 << "==> Total chisquare/dofs = " << m_equations->totalChiSquare() << " / " << m_equations->totalNumDofs() << std::endl
	 << "==> Total number of hits in external detectors = " << m_equations->numExternalHits() << endmsg;

  for (size_t i = 0; i < m_equations->nElem(); ++i) {
    for (size_t j = i; j < m_equations->nElem(); ++j) {
      info() << "==> M["<<i<<","<<j<<"] = "      << m_equations->M(i,j) << endmsg;
    }
    info() << "\n==> V["<<i<<"] = "    << m_equations->V(i) << endmsg;    
  }

  /// Take the gaudi vector and matix and fill AlVec and AlSym
  AlVec    tmpDerivatives(Derivatives::kCols*(m_equations->nElem()));
  AlSymMat tmpMatrix(Derivatives::kCols*(m_equations->nElem()));

  /// Loop over map of index to 2nd derivative matrix and 1st derivative vector
  for (unsigned i = 0u, iEnd = m_equations->nElem(); i < iEnd ; ++i) {
    ass(tmpDerivatives, i*Derivatives::kCols, m_equations->V(i));
    /// (assume upper triangular input!)
    for (unsigned j = i ; j < iEnd; ++j ) ass(tmpMatrix, i*Derivatives::kCols, j*Derivatives::kCols, m_equations->M(i,j));
  }
  //   for (unsigned i = 0u, iEnd = m_equations->nElem(); i < iEnd ; ++i) {
  //     tmpDerivatives.insert(i*Derivatives::kCols, m_equations->V(i));
  //     /// (assume upper triangular input!)
  //     for (unsigned j = i ; j < iEnd; ++j) tmpMatrix.insert(i*Derivatives::kCols, j*Derivatives::kCols, m_equations->M(i,j));
  //   }

  /// Define new AlVec and AlSymMat corresponding to the number of degrees of freedom we want to align
  assert(tmpMatrix.size() == m_dofMask.size() || tmpDerivatives.size() == m_dofMask.size());
  AlVec    derivatives(m_nDoFs);
  AlSymMat matrix(m_nDoFs);
  
  unsigned insert_i = 0u;
  unsigned insert_j = 0u;
  typedef std::vector<bool>::const_iterator boolIter; 
  for (std::vector<bool>::const_iterator i = m_dofMask.begin(), iEnd = m_dofMask.end(); i != iEnd; ++i) {
    if (*i) {
      const unsigned index_i = std::distance(boolIter(m_dofMask.begin()), i);
      derivatives[insert_i] = tmpDerivatives[index_i];
      for (std::vector<bool>::const_iterator j = i, jEnd = m_dofMask.end(); j != jEnd; ++j) {
        if (*j) {
          if (printDebug()) {
            debug() << "insert_i = " << insert_i << " insert_j = " << insert_j 
                    << " matrix[" << index_i << "][" << std::distance(boolIter(m_dofMask.begin()), j) << "]" 
                    << tmpMatrix[index_i][std::distance(boolIter(m_dofMask.begin()), j)] << endmsg;
          }
          matrix[insert_i][insert_j++] = tmpMatrix[index_i][std::distance(boolIter(m_dofMask.begin()), j)];
        }
      }
      insert_j = ++insert_i;
    }
  }
  
  if (printDebug()) {
    debug() << "Tmp AlVec Vector    = " << tmpDerivatives << endmsg;
    debug() << "Tmp AlSymMat Matrix = " << tmpMatrix      << endmsg;
  }
  
  info() << "AlVec Vector    = " << derivatives << endmsg;
  info() << "AlSymMat Matrix = " << matrix      << endmsg;
  
  /// Add constraints
//   if (!m_constraints.empty()) {
//     for (unsigned i = Derivatives::kCols*numElements, iEnd = Derivatives::kCols*numElements + m_constraints.size(),
//            nC = 0u; i != iEnd; ++i, ++nC) {
//       const std::vector<double>& constraint = m_constraints.at(nC);
//       derivatives[i] = constraint.back();
//       unsigned cEntry = 0u;
//       for (std::vector<double>::const_iterator j = constraint.begin(), jEnd = constraint.end()-1; j != jEnd; ++j, ++cEntry) {
//         matrix[i][cEntry] = (*j);
//       }
//     }
//   }
  
//   info() << "AlVec Vector = " << derivatives << endmsg;
//   info() << "AlSymMat Matrix = " << matrix << endmsg;


  // Tool returns H^-1 and alignment constants. Need to copy the 2nd derivative because we still need it.
  AlSymMat dChi2dX = matrix ;
    
  bool solved = m_matrixSolverTool->compute(matrix, derivatives);
  if (solved) {
    StatusCode sc = StatusCode::SUCCESS;
    info() << "Solution = " << derivatives << endmsg;
    info() << "Chisquare/dof of the alignment change: " 
	   << derivatives * dChi2dX * derivatives << " / " << m_nDoFs << endmsg ;
    
    /// Update alignment iff we've solved Ax=b
    if (printDebug()) debug() << "==> Putting alignment constants" << endmsg;
    if (derivatives.size() != m_dofMask.size()) {
      AlVec tmp(Derivatives::kCols*(m_equations->nElem()));
      insert_i = 0u;
      unsigned index_j = 0u;
      for (std::vector<bool>::const_iterator i = m_dofMask.begin(), iEnd = m_dofMask.end(); i != iEnd; ++i) {
        (*i) ? tmp[insert_i++] = derivatives[index_j++] : tmp[insert_i++] = 0.0;
      }
      if (printDebug()) debug() << "==> putting alignment deltas = " << tmp << endmsg;
      sc = putAlignmentConstants(m_rangeElements, tmp);
    } else {
      if (printDebug()) debug() << "==> putting alignment deltas = " << derivatives << endmsg;
      sc = putAlignmentConstants(m_rangeElements, derivatives);
    }
    if (sc.isFailure()) error() << "==> Failed to put alignment constants" << endmsg;
  } else {
    error() << "Failed to solve system!" << endmsg;
  }
}

void AlignAlgorithm::reset() {
  if (printDebug()) debug() << "increasing iteration counter and resetting accumulated data..." << endmsg;
  /// increment iteration counter
  ++m_iteration;
  // clear derivatives and H maps
  m_equations->clear();
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

void AlignAlgorithm::getAlignmentConstants(const Range& rangeElements, AlignConstants& alignConstants) const {
  alignConstants.reserve(6*std::distance(rangeElements.first, rangeElements.second)); /// 6 constants * num elements
  for (ElemIter i = rangeElements.first, iEnd = rangeElements.second; i != iEnd; ++i) {
    if (printDebug()) debug() << "Getting alignment constants for " << (*i) << endmsg;
    /// Get translations and rotations
    const std::vector<double>& translations = i->deltaTranslations();
    const std::vector<double>& rotations    = i->deltaRotations();
    /// Insert intitial constants (per element)
    alignConstants.insert(alignConstants.end(), translations.begin(), translations.end());
    alignConstants.insert(alignConstants.end(), rotations.begin()   , rotations.end()   );
  }
}

StatusCode AlignAlgorithm::putAlignmentConstants(const Range& rangeElements, const AlVec& alignConstants) const
{
  if (printDebug()) debug() << "Size of alignment constants vector is " << alignConstants.size() << endmsg;
  for (ElemIter i = rangeElements.first, iEnd = rangeElements.second; i != iEnd; ++i) {
    if (printDebug()) { debug() << i->name()
      //<< "\n current local delta : \n" << i->localDeltaMatrix()
				<< " current local centre (0,0,0) in global coordinates: "
				<< i->localCentre() << endmsg;
    }
    /// 3 translations
    std::vector<double> translations;
    for (unsigned j = 0u; j < 3u; ++j) translations.push_back(alignConstants[i->index()*Derivatives::kCols + j]);
    /// 3 rotations
    std::vector<double> rotations;
    for (unsigned j  = 3u; j < 6u; ++j) rotations.push_back(alignConstants[i->index()*Derivatives::kCols + j]);
    /// Set local delta matrix
    StatusCode sc = i->setLocalDeltaMatrix(translations, rotations);
    if (sc.isFailure()) return Error("Failed to set alignment conditions", StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}
