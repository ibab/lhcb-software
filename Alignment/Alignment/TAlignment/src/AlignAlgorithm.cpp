// $Id: AlignAlgorithm.cpp,v 1.5 2007-07-20 17:24:33 janos Exp $
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
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/GlobalToLocalDelta.h"
#include "DetDesc/Condition.h"
#include "DetDesc/AlignmentCondition.h"

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
#include "boost/lexical_cast.hpp"

// AIDA
#include "AIDA/IHistogram2D.h"

// local
#include "AlignAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignAlgorithm
//
// 2007-03-05 : Jan Amoraal
//-----------------------------------------------------------------------------

using namespace LHCb;
using namespace boost;
using boost::lexical_cast;

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
                                m_alignDetector(NULL),
                                m_tracksLocation(""),
                                m_projSelectorName(""),
                                m_projSelector(NULL),
                                m_matrixSolverToolName(""),               
                                m_matrixSolverTool(NULL)
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
  /// 1x6 vector
  debug() << "==> Resizing Alvec" << endmsg;
  m_derivatives.reSize(m_nAlignConstants*m_nDetElements);
  //m_oldDerivatives.reSize(m_nAlignConstants*m_nDetElements);
  debug() << "==> Done resizing Alvec" << endmsg;  
  /// 6x6 matrix
  debug() << "==> Resizing AlSymMat" << endmsg;
  m_hMatrix.reSize(m_nAlignConstants*m_nDetElements);
  debug() << "==> Done resizing AlSymMat" << endmsg;
  debug() << "Number of elements to align = " << m_nDetElements << endmsg;

  /// Get pivot points
  sc = getPivotPoints(m_elements, m_pivotPoints);
  if (sc.isFailure()) return Error("==> Failed to get pivot points or not defined", StatusCode::FAILURE);
  
  /// Get alignment constants
  debug() << "==> Retrieving alignment paramterers" << endmsg;
  debug() << "==> Expect " << m_nAlignConstants*m_elements.size() << " station alignment constants" << endmsg;
  sc = getAlignmentConstants(m_elements, m_initAlignConstants);
  if (sc.isFailure()) return Error("==> Failed to retrieve initial alignment constants", StatusCode::FAILURE);
  debug() << "==> Got " << m_initAlignConstants.size() << " detector elements alignment constants" << endmsg;
  debug() << "==> Initial alignment constants are: " <<   m_initAlignConstants << endmsg;

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
  for (ElemIter iE = m_elements.begin(), iEend = m_elements.end(); 
       iE != iEend; ++nE, ++iE) {
    unsigned int id = 1000u + nE;
    std::string indexToString = boost::lexical_cast<std::string>(id);
    resHistos[id] = book2D(id, "residual vs iteration for element " + indexToString, -0.5, m_nIterations-0.5, m_nIterations, -1.00 , +1.00, 100); 
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
      /// Check if node has a measurement
      if (!((*iN)->hasMeasurement())) {
        debug() << "==> Node has no measurement" << endmsg;
        continue;
      }
      /// Get measurement
      const Measurement& meas = (*iN)->measurement();
      /// Get LHCb ID
      const LHCbID& id        = meas.lhcbID();
      /// Check if LHCb is valid and get index associated to DetElem
      typedef std::pair<bool, unsigned int> ValidIndex;
      ValidIndex elementIndex = m_alignDetector->index(id);
      bool valid              = elementIndex.first;
      unsigned int index      = elementIndex.second;
      if (!valid) {
        debug() << "==> Measurement not on a to-be-aligned DetElem" << endmsg;
        continue;
      }
      if (printDebug()) debug() << "==> Index = " << index << endmsg;
      double res  = (*iN)->residual();
      double err2 = (*iN)->errResidual2();
      resHistos[1000 + index]->fill(m_iteration, res); 
      // Get projector for this measurement
      ITrackProjector* proj = m_projSelector->projector(meas);
      if (!proj) {
        debug() << "==> Could not get pojector for measurement!" << endmsg;
        continue;
      }
      Derivatives der = proj->alignmentDerivatives(meas, m_pivotPoints[index]);
      /// Map index to H matrix and derivatives 
      m_equations[unsigned(index)].first  += (Transpose(der)*der)/err2; 
      m_equations[unsigned(index)].second += -1.0*((res*der)/err2); 
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
    
  info() << "AlSymMat size = " << m_derivatives.size() << endmsg;
  info() << "AlSymMat size = " << m_hMatrix.size() << endmsg;

  /// Loop over map of index to H matrix and derivatives 
  for (unsigned(i) = 0u, iEnd = m_equations.size(); i < iEnd ; ++i) {                                           
    for (unsigned(j) = 0u; j < m_nAlignConstants; ++j) {
      /// Fill AlVec
      m_derivatives[i*m_nAlignConstants+j] = (m_equations[i].second)(0,j);
      /// Fill AlSymMat
      for (unsigned(k) = 0u; k < m_nAlignConstants; ++k) {
        m_hMatrix[i*m_nAlignConstants+j][i*m_nAlignConstants+k] = (m_equations[i].first)(j,k);
      }
    }
  }

  info() << "AlVec Vector = " << m_derivatives << endmsg;
  info() << "AlSymMat Matrix = " << m_hMatrix << endmsg;
  
  /// Tool returns H^-1 and alignment constants 
  bool solved = m_matrixSolverTool->compute(m_hMatrix, m_derivatives);  
  if (solved) {
    info() << "solved it" << endmsg;
    info() << "Constants Element = " << m_derivatives << endmsg;
    info() << "H^-1 Matrix = "  << m_hMatrix << endmsg;
    for (unsigned(i) = 0u; i < m_hMatrix.size(); ++i) {
      info() << "AlignConstant/sqrt(" << "[" << i << "]" << "[" << i << "]" << ") = " << m_derivatives[i]/std::sqrt(m_hMatrix[i][i]) << endmsg; 
    }
  } else {
    error() << "Failed to solve system" << endmsg;
  }

  /// Update alignement constants (in memory)
  debug() << "==> Putting alignment constants" << endmsg;
  StatusCode sc = putAlignmentConstants(m_elements, m_derivatives);
  if (sc.isFailure()) error() << "==> Failed to put alignment constants" << endmsg;
}

void AlignAlgorithm::reset() {
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
    if (!alignCond) return Error("==> Failed to get pivot points", StatusCode::FAILURE);
    /// Get pivot point
    std::vector<double> pivotPoint = alignCond->paramVect<double>("pivotXYZ");
    /// Insert pivot point
    pivotPoints.push_back(Gaudi::XYZPoint(pivotPoint[0], pivotPoint[1], pivotPoint[2]));
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
    std::vector<double> translations;
    std::vector<double> rotations;
    /// 3 translations
    for (unsigned(i) = 0u; i < 3u; ++i) translations.push_back(alignConstants[nE*m_nAlignConstants + i]);
    /// 3 rotations
    for (unsigned(i) = 3u; i < 6u; ++i) rotations.push_back(alignConstants[nE*m_nAlignConstants + i]);
    /// Pivot point
    std::vector<double> pivot(3,0.0);
    /// Get transformation matrix to go from old delta to new delta
    const Gaudi::Transform3D globalDeltaMatrix = DetDesc::localToGlobalTransformation(translations, rotations, pivot);
    /// Transform global deltas to local deltas
    const Gaudi::Transform3D localDeltaMatrix  = DetDesc::localDeltaMatrix((*iE), globalDeltaMatrix);
    /// Update position of detector elements
    StatusCode sc = (*iE)->geometry()->localDeltaMatrix(localDeltaMatrix); 
    if (sc.isFailure()) return Error("Failed to set alignment conditions", StatusCode::FAILURE);
    /// Clear translations and rotations for next detector element
    translations.clear();
    rotations.clear();
  }
  return StatusCode::SUCCESS;
}
