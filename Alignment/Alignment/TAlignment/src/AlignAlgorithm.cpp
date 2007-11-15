// $Id: AlignAlgorithm.cpp,v 1.11 2007-11-15 15:22:54 janos Exp $
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
       Equations(size_t nElem) : m_nElem(nElem), m_v(nElem) {}
       size_t nElem() const { return m_nElem; }
       Gaudi::Vector6&   V(int i) { return m_v[i]; }
       const Gaudi::Vector6&   V(int i) const { return m_v[i]; }
       Gaudi::Matrix6x6& M(int i, int j) { assert(i<=j); return m_m[std::make_pair(i,j)]; }
       //FIXME: return proxy that fixes i<j...
       const Gaudi::Matrix6x6& M(int i, int j) const { assert(i<=j); return const_cast<Equations*>(this)->m_m[std::make_pair(i,j)]; }
       void clear() { m_v.clear(); m_v.resize(m_nElem); m_m.clear(); }
   private:
       size_t m_nElem ;
       std::vector<Gaudi::Vector6>                    m_v;
       std::map<std::pair<int,int>, Gaudi::Matrix6x6> m_m;
   };

   Gaudi::Vector6 convert(const Gaudi::Matrix1x6& m) {
        Gaudi::Vector6 v;
        for (int i = 0; i < 6; ++i) v(i) = m(0,i);
        return v;
   };


   class Data {
   public:
        Data(LHCb::LHCbID id, unsigned index, double r, const Gaudi::Matrix1x6& d) 
        : m_id(id),m_index(index),m_r(r),m_d(d) {}
        LHCb::LHCbID id() const { return m_id ; }
        unsigned index() const { return m_index; }
        double r() const { return m_r; }
        const Gaudi::Matrix1x6& d() const { return m_d; }
   private:
        LHCb::LHCbID     m_id;
        unsigned         m_index;
        double           m_r;
        Gaudi::Matrix1x6 m_d;
   };


   //FIXME: the next two stand-alone functions should move into AlVec & AlSymMat...
   template <typename T>
   void ass(AlVec& lhs, unsigned row, const T& rhs) {
        for (unsigned i=0;i<T::kSize;++i) lhs[row+i] = rhs(i);
   }

   template <typename T>
   void ass(AlSymMat& lhs, unsigned col, unsigned row, const T& rhs) {
        for (unsigned i=0;i<T::kCols;++i) 
            for (unsigned j=0;j<T::kRows;++j) 
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
				  m_nTracks(0u),
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

  if (printDebug()) {
    debug() << "==> Got " << std::distance(m_rangeElements.first, m_rangeElements.second) << " elements to align!" << endmsg;
    for (ElemIter i = m_rangeElements.first, iEnd = m_rangeElements.second; i != iEnd; ++i) {
      debug()<<  "        " << i->index() << " : " << i->name() << "\n" << endmsg;
    }
  }
  
  m_equations = new Equations(std::distance(m_rangeElements.first, m_rangeElements.second));

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

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignAlgorithm::execute() {

  // Get tracks 
  Tracks* tracks = get<Tracks>(m_tracksLocation);
  if (printDebug()) debug() << "Number of tracks in container " + m_tracksLocation + ": " << tracks->size() << endmsg;
  m_nTracks += tracks->size();
  /// Loop over tracks
  typedef Tracks::const_iterator TrackIter; 
  for (TrackIter iT = tracks->begin(), iTend = tracks->end(); iT != iTend; ++iT) {
    std::vector<Data> data;
    // Get nodes. Need them for measurements, residuals and errors
    const Nodes& nodes = (*iT)->nodes();
    if (printDebug()) debug() << "==> Found " << nodes.size() << " nodes"<< endmsg;

    // Loop over nodes and get measurements, residuals and errors
    typedef Nodes::const_iterator NodeIter;
    for (NodeIter node = nodes.begin(), end = nodes.end(); node != end; ++node) {
      if (!(*node)->hasMeasurement()) {
        if (printDebug()) debug() << "==> Node has no measurement" << endmsg;
        continue;
      }
      /// Get measurement
      const Measurement& meas = (*node)->measurement();
      /// Get element that belongs to this measurment 
      const AlignmentElement* elem = m_align->findElement(meas.lhcbID());
      if (!elem) {
        if (printDebug()) debug() << "==> Measurement not on a to-be-aligned DetElem" << endmsg;
        continue;
      }
      const unsigned index = elem->index();
      if (printDebug()) debug() << "==> " << meas.lhcbID() << " -> index = " << index << " -> " <<  elem->name() << endmsg;
      /// Project measurement
      ITrackProjector* proj = m_projSelector->projector(meas);
      if (!proj) {
        if (printDebug()) debug() << "==> Could not get projector for selected measurement!" << endmsg;
        continue;
      }
      double res  = (*node)->residual();
      double err  = (*node)->errMeasure();

      m_nhitHistos[index]->fill(m_iteration); 
      m_resHistos[index]->fill(m_iteration, res); 
      m_pullHistos[index]->fill(m_iteration, res/err); 
      // Get alignment derivatives
      Derivatives der = proj->alignmentDerivatives(meas, elem->pivotXYZPoint());
      // push back normalized residuals & derivatives;
      res/=err;
      der/=err;
      data.push_back(Data(meas.lhcbID(), index, res, der));
    }

    ResidualCovarianceTool cov;
    cov.compute(**iT);


    for (std::vector<Data>::const_iterator id = data.begin(), idEnd = data.end(); id != idEnd; ++id) {
      m_equations->V(id->index())              -= convert(id->r()*id->d()) ; 
      m_equations->M(id->index(), id->index()) += (Transpose(id->d())*id->d());
      
      for (std::vector<Data>::const_iterator jd = id; jd != idEnd; ++jd) {
	if ( m_correlation || jd==id) {
	  // make sure we go for the upper triangle...
	  std::vector<Data>::const_iterator i(id), j(jd);
	  if (i->index() > j->index()) std::swap(i, j);
	  const double c = cov.HCH_norm(i->id(),j->id());
	  m_equations->M(i->index(), j->index()) -= c * Transpose(i->d())*j->d();
	  
	  if (!( i->id()==j->id())) {
	    m_corrHistos[std::make_pair(i->index(), j->index())]->fill(m_iteration, c/std::sqrt(cov.HCH_norm(i->id(), i->id())*cov.HCH_norm(j->id(), j->id())));
	  } else {
	    m_autoCorrHistos[i->index()]->fill(m_iteration,c);
	  }
	}
      }
    }
    
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Update
//=============================================================================
void AlignAlgorithm::update() {
  
  info() <<   "==> Updating constants" 
         << "\n==> Used " << m_nTracks << " tracks for alignment" << endmsg;
  
  for (size_t i = 0; i < m_equations->nElem(); ++i) {
    for (size_t j = i; j < m_equations->nElem(); ++j) {
       info() << "==> M["<<i<<","<<j<<"] = "      << m_equations->M(i,j) << endmsg;
    }
    info() << "\n==> V["<<i<<"] = "    << m_equations->V(i) << endmsg;
  }  
    

  const size_t numElements = std::distance(m_rangeElements.first, m_rangeElements.second);
  
  AlVec    derivatives(Derivatives::kCols*numElements + m_constraints.size());
  AlSymMat      matrix(Derivatives::kCols*numElements + m_constraints.size());

  /// Loop over map of index to 2nd derivative matrix and 1st derivative vector 
  for (unsigned i = 0u, iEnd = m_equations->nElem(); i < iEnd ; ++i) {                                           
    ass(derivatives, i*Derivatives::kCols, m_equations->V(i));
    /// (assume upper triangular input!)
    for (unsigned j = i ; j < iEnd ; ++j) ass(matrix, i*Derivatives::kCols, j*Derivatives::kCols, m_equations->M(i,j));
  }

  /// Add constraints
  if (!m_constraints.empty()) {
    for (unsigned i = Derivatives::kCols*numElements, iEnd = Derivatives::kCols*numElements + m_constraints.size(), 
	   nC = 0u; i != iEnd; ++i, ++nC) {
      const std::vector<double>& constraint = m_constraints.at(nC);
      derivatives[i] = constraint.back();
      unsigned cEntry = 0u;
      for (std::vector<double>::const_iterator j = constraint.begin(), jEnd = constraint.end()-1; j != jEnd; ++j, ++cEntry) {
        matrix[i][cEntry] = (*j);
      }
    }
  }

  info() << "AlVec Vector = " << derivatives << endmsg;
  info() << "AlSymMat Matrix = " << matrix << endmsg;
  
  /// Tool returns H^-1 and alignment constants 
  bool solved = m_matrixSolverTool->compute(matrix, derivatives);  
  if (solved) {
    info() << "Solution = " << derivatives << endmsg;
    /// Update alignment iff we've solved Ax=b 
    if (printDebug()) debug() << "==> Putting alignment constants" << endmsg;
    StatusCode sc = putAlignmentConstants(m_rangeElements, derivatives);
    if (sc.isFailure()) error() << "==> Failed to put alignment constants" << endmsg;
  } else {
    error() << "Failed to solve system!" << endmsg;
  }
}

void AlignAlgorithm::reset() {
  if (printDebug()) debug() << "increasing iteration counter and resetting accumulated data..." << endmsg;
  /// increment iteration counter
  ++m_iteration;
  /// reset track counters
  m_nTracks = 0u;
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
                                << "\n current local delta : \n" << i->localDeltaMatrix()
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
