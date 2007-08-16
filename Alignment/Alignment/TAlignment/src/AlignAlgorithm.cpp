// $Id: AlignAlgorithm.cpp,v 1.6 2007-08-16 13:52:41 graven Exp $
// Include files
// from std
#include <utility>
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
#include "DetDesc/ParamException.h"

// from Kernel
#include "Kernel/LHCbID.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/Node.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackProjectorSelector.h"
#include "TrackInterfaces/ITrackProjector.h"

// from AlignmentInterfaces
#include "AlignmentInterfaces/IAlignDetectorSelector.h"
#include "AlignmentInterfaces/IAlignSolvTool.h"

// from BOOST
#include "boost/tuple/tuple.hpp"
#include "boost/assign/list_of.hpp"


// AIDA
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram1D.h"

// local
#include "AlignAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignAlgorithm
//
// 2007-03-05 : Jan Amoraal
//-----------------------------------------------------------------------------

using namespace LHCb;
using namespace boost;
using boost::tie;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AlignAlgorithm );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlignAlgorithm::AlignAlgorithm( const std::string& name,
                                ISvcLocator* pSvcLocator)
                                : GaudiHistoAlg( name , pSvcLocator ),
                                m_iteration(0u),
                                m_nIterations(0u),
                                m_elements(),
                                m_nDetElements(0u),
                                m_nAlignConstants(0u),
                                m_nTracks(0u),
                                m_initAlignConstants(),
                                m_pivotPoints(),
                                m_alignDetectorName(""),
                                m_alignDetector(0),
                                m_tracksLocation(""),
                                m_projSelectorName(""),
                                m_projSelector(0),
                                m_matrixSolverToolName(""),               
                                m_matrixSolverTool(0)
{
  declareProperty("NumberOfIterations"   , m_nIterations          = 10u                     );
  declareProperty("AlignDetectorSelector", m_alignDetectorName    = "AlignOTDetector"       );
  declareProperty("TracksLocation"       , m_tracksLocation       = TrackLocation::Default  );
  declareProperty("ProjectorSelector"    , m_projSelectorName     = "TrackProjectorSelector");
  declareProperty("MatrixSolverTool"     , m_matrixSolverToolName = "SpmInvTool"            ); 
} 
//=============================================================================
// Destructor
//=============================================================================
AlignAlgorithm::~AlignAlgorithm() {} 

//=============================================================================
// Initialization
//=============================================================================
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
  m_alignDetector = tool<IAlignDetectorSelector>(m_alignDetectorName, "Detector", this);
  if (!m_alignDetector) return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);
  
  /// Get detector elements
  m_elements = m_alignDetector->getDetectorElements();
  debug() << "Got " << m_elements.size() << " elements" << endmsg;
  /// Get number of elements
  m_nDetElements = m_alignDetector->nElements();
  m_nAlignConstants = Derivatives::kCols;
  debug() << "Number of elements to align = " << m_nDetElements << endmsg;

  // get constraints
  m_constraints = m_alignDetector->constraints();
  info() << "Number of constraints = " << m_constraints.size() << endmsg;
  unsigned nC = 0u;
  for (std::vector<std::vector<double> >::const_iterator i = m_constraints.begin(), iEnd = m_constraints.end(); i != iEnd; 
       ++i, ++nC) {
    info() << nC << ": f = [ ";
    for (std::vector<double>::const_iterator j = (*i).begin(), jEnd = (*i).end()-1; j != jEnd; ++j) {
    info()  << (*j) << " ";
    }
    info() << " ]" << endmsg;
    info() << "   f0 = " << (*i).back() << endmsg;
  }  

  /// Get pivot points
  sc = getPivotPoints(m_elements, m_pivotPoints);
  if (sc.isFailure()) return Error("==> Failed to get pivot points or not defined", StatusCode::FAILURE);
  
  /// Get projector selector tool
  m_projSelector = tool<ITrackProjectorSelector>(m_projSelectorName, "Projector", this);
  if (!m_projSelector) return Error("==> Failed to retrieve projector selector tool", StatusCode::FAILURE);
  
  ///Get matrix solver tool
  m_matrixSolverTool = tool<IAlignSolvTool>(m_matrixSolverToolName, "MatrixSolver", this);
  if (!m_matrixSolverTool) return Error("==> Failed to retrieve matrix solver tool", StatusCode::FAILURE);

  /// Monitoring
  /// Book residual histograms
  /// Residuals
  //unsigned int nE = 0u; 
  unsigned(nE) = 0u;
  for (ElemIter i = m_elements.begin(), end = m_elements.end(); 
       i != end; ++nE, ++i) {
    m_resHistos[nE] = book2D(1000u+nE, "residual vs iteration for " + (*i)->name(),
                           -0.5, m_nIterations-0.5, m_nIterations, 
                           -1.00 , +1.00, 100); 
    m_pullHistos[nE] = book2D(2000u+nE, "pull vs iteration for " + (*i)->name(),
                           -0.5, m_nIterations-0.5, m_nIterations, 
                           -5.00 , +5.00, 100); 
    m_nhitHistos[nE] = book1D(3000u+nE, "number of hits vs iteration for " + (*i)->name(),
                           -0.5, m_nIterations-0.5, m_nIterations);
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignAlgorithm::execute() {
  // Get tracks 
  Tracks* tracks = get<Tracks>(m_tracksLocation);
  if (printDebug()) debug() << "Number of tracks in container " + m_tracksLocation + " " << tracks->size() << endmsg;
  m_nTracks += tracks->size();
  /// Loop over tracks
  typedef Tracks::const_iterator TrackIter; 
  for (TrackIter iT = tracks->begin(), iTend = tracks->end(); iT != iTend; ++iT) {
    // Get nodes. Need them for measurements, residuals and errors
    const Nodes& nodes = (*iT)->nodes();
    if (printDebug()) debug() << "==> Found " << nodes.size() << " nodes"<< endmsg;
    // Loop over nodes and get measurements, residuals and errors
    typedef Nodes::const_iterator NodeIter;
    for (NodeIter iN = nodes.begin(), iNend = nodes.end(); iN != iNend; ++iN) {
      if (!(*iN)->hasMeasurement()) {
        debug() << "==> Node has no measurement" << endmsg;
        continue;
      }
      /// Get measurement
      const Measurement& meas = (*iN)->measurement();
      /// Check if LHCb is valid and get index associated to DetElem
      bool valid(false); unsigned int index(0);
      tie(valid,index) = m_alignDetector->index(meas.lhcbID());
      if (!valid) {
        debug() << "==> Measurement not on a to-be-aligned DetElem" << endmsg;
        continue;
      }
//      std::cout << "==> " << meas.lhcbID() << " -> index = " << index << " -> " << m_alignDetector->getDetectorElements()[index]->name() << std::endl;
      if (printDebug()) {
          debug() << "==> " << meas.lhcbID() << " -> index = " << index << " -> " << m_alignDetector->getDetectorElements()[index]->name() << endmsg;
      }
      ITrackProjector* proj = m_projSelector->projector(meas);
      if (!proj) {
        debug() << "==> Could not get pojector for measurement!" << endmsg;
        continue;
      }
      double res  = (*iN)->residual();
      double err2 = (*iN)->errResidual2();
      m_nhitHistos[index]->fill(m_iteration); 
      m_resHistos[index]->fill(m_iteration, res); 
      m_pullHistos[index]->fill(m_iteration, res/sqrt(err2)); 
      // Get projector for this measurement
      Derivatives der = proj->alignmentDerivatives(meas, m_pivotPoints[index]);
//      std::cout << "got H=" << der << " res = " << res << " err = " << sqrt(err2) << std::endl;
      /// Map index to H matrix and derivatives 
      m_equations[index].first  += (Transpose(der)*der)/err2; 
      m_equations[index].second += -1.0*((res*der)/err2); 
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Update
//=============================================================================
void AlignAlgorithm::update() {
  
  info() << "==> Updating constants" << endmsg;
  info() << "==> Used " << m_nTracks << " tracks for alignment" << endmsg;
  
  typedef Equations::const_iterator EqIter;
  for (EqIter iEq = m_equations.begin(), iEqEnd = m_equations.end(); iEq != iEqEnd; ++iEq) {
    info() << "==> Element: " << iEq->first << endmsg;
    info() << "==> H = " << (iEq->second).first << endmsg;
    info() << "==> rho = " << (iEq->second).second << endmsg;
  }  
    

  AlVec    derivatives(m_nAlignConstants*m_nDetElements + m_constraints.size());
  AlSymMat matrix(m_nAlignConstants*m_nDetElements + m_constraints.size());

  debug() << "AlSymMat size = " << derivatives.size() << endmsg;
  debug() << "AlSymMat size = " << matrix.size() << endmsg;

  /// Loop over map of index to H matrix and derivatives 
  for (unsigned(i) = 0u, iEnd = m_equations.size(); i < iEnd ; ++i) {                                           
    for (unsigned(j) = 0u; j < m_nAlignConstants; ++j) {
      /// Fill AlVec
      derivatives[i*m_nAlignConstants+j] = (m_equations[i].second)(0,j);
      /// Fill AlSymMat
      for (unsigned(k) = 0u; k < m_nAlignConstants; ++k) {
        matrix[i*m_nAlignConstants+j][i*m_nAlignConstants+k] = (m_equations[i].first)(j,k);
      }
    }
  }

  /// Add constraints
  if (!m_constraints.empty()) {
    for (unsigned(i) = m_nAlignConstants*m_nDetElements, iEnd = m_nAlignConstants*m_nDetElements + m_constraints.size(), nC=0u;  
         i != iEnd; ++i, ++nC) {
      const std::vector<double>& constraint = m_constraints.at(nC);
      derivatives[i] = constraint.back();
      unsigned cEntry = 0u;
      for (std::vector<double>::const_iterator j = constraint.begin(), jEnd = constraint.end()-1; j != jEnd; ++j, ++cEntry) {
        matrix[i][cEntry] = (*j);
      }
    }
  }

  debug() << "AlVec Vector = " << derivatives << endmsg;
  debug() << "AlSymMat Matrix = " << matrix << endmsg;
  
  /// Tool returns H^-1 and alignment constants 
  bool solved = m_matrixSolverTool->compute(matrix, derivatives);  
  if (solved) {
    info() << "solved it" << endmsg;
    info() << "Constants Element = " << derivatives << endmsg;
    /// Update alignment iff we've solved Ax=b 
    debug() << "==> Putting alignment constants" << endmsg;
    StatusCode sc = putAlignmentConstants(m_elements, derivatives);
    if (sc.isFailure()) error() << "==> Failed to put alignment constants" << endmsg;
  } else {
    error() << "Failed to solve system" << endmsg;
  }
}

void AlignAlgorithm::reset() {
  debug() << "increasing iteration counter and resetting accumulated data..." << endmsg;
  /// increment iteration counter
  ++m_iteration;
  /// reset track counters
  m_nTracks = 0u;
  // clear derivatives and H maps
  m_equations.clear();
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

StatusCode AlignAlgorithm::getPivotPoints(const Elements& elements, PivotPoints& pivotPoints) const
{
  const AlignmentCondition *alignCond;
  pivotPoints.reserve(elements.size());
  for (ElemIter iE = elements.begin(), iEend = elements.end(); iE != iEend; ++iE) {
    if (printDebug()) debug() << (*iE)->name() << endmsg;
    /// Get alignment condition
    alignCond = (*iE)->geometry()->alignmentCondition();
    if (!alignCond) return Error("==> Failed to get alignment condition!", StatusCode::FAILURE);
    /// Get pivot point
    /// Set default point
    Gaudi::XYZPoint pivotPoint = (*iE)->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 0.0, 0.0));
    try { /// If pivot is defined for this det elem get it
      std::vector<double> pivot = alignCond->paramVect<double>("pivotXYZ");
      pivotPoint = Gaudi::XYZPoint(pivot[0], pivot[1], pivot[2]);
      info() << "==> Pivot point explicity defined for " << (*iE)->name() 
             << ". Using " << pivotPoint << " as pivot point" << endmsg; 
    } catch (ParamException& p) { /// Print info and use default
      info() << "==> Pivot point not explicity defined for " << (*iE)->name() 
             << ". Using global centre of element, " << pivotPoint << ", as pivot point" << endmsg; 
    }
    /// Insert pivot point
    std::cout<< "using pivot " << pivotPoint << " for " << (*iE)->name() << std::endl;
    pivotPoints.push_back(pivotPoint);

  }
  return StatusCode::SUCCESS;
}

StatusCode AlignAlgorithm::getAlignmentConstants(const Elements& elements, AlignConstants& alignConstants) const
{
  const AlignmentCondition *alignCond;
  alignConstants.reserve(6*elements.size()); /// 9 constants * num elements
  for (ElemIter iE = elements.begin(), iEend = elements.end(); iE != iEend; ++iE) {
    if (printDebug()) debug() << (*iE)->name() << endmsg;
    /// Get alignment condition
    alignCond = (*iE)->geometry()->alignmentCondition();
    if (!alignCond) return Error("==> Failed to get alignment conditions", StatusCode::FAILURE);
    /// Get translations and rotations
    std::vector<double> translations = alignCond->paramVect<double>("dPosXYZ");
    std::vector<double> rotations    = alignCond->paramVect<double>("dRotXYZ");
    /// Insert intitial constants (per element)
    alignConstants.insert(alignConstants.end(), translations.begin(), translations.end());
    alignConstants.insert(alignConstants.end(), rotations.begin(), rotations.end());
  }
  return StatusCode::SUCCESS;
}

StatusCode AlignAlgorithm::putAlignmentConstants(const Elements& elements, const AlVec& alignConstants) const
{
  if (printDebug()) debug() << "Size of alignment constants vector is " << alignConstants.size() << endmsg;
  unsigned(nE) = 0u;
  for (ElemIter iE = elements.begin(), iEend = elements.end(); iE != iEend; ++nE, ++iE) {
    if (printDebug()) debug() << (*iE)->name() << endmsg;
    /// 3 translations
    std::vector<double> translations;
    for (unsigned(i) = 0u; i < 3u; ++i) translations.push_back(alignConstants[nE*m_nAlignConstants + i]);
    /// 3 rotations
    std::vector<double> rotations;
    for (unsigned(i) = 3u; i < 6u; ++i) rotations.push_back(alignConstants[nE*m_nAlignConstants + i]);
    /// Pivot point
    Gaudi::XYZPoint p = m_pivotPoints[nE];
    std::vector<double> pivot = boost::assign::list_of(p.x())(p.y())(p.z());
    /// Get transformation matrix to go from old delta to new delta
    const Gaudi::Transform3D globalDeltaMatrix = DetDesc::localToGlobalTransformation(translations, rotations, pivot);
    /// Transform global deltas to local deltas
    const Gaudi::Transform3D localDeltaMatrix  = DetDesc::localDeltaMatrix((*iE), globalDeltaMatrix);
    /// Update position of detector elements
    StatusCode sc = (*iE)->geometry()->localDeltaMatrix(localDeltaMatrix); 
    if (sc.isFailure()) return Error("Failed to set alignment conditions", StatusCode::FAILURE);
  }
  return StatusCode::SUCCESS;
}
