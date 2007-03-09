// $Id: AlignAlgorithm.cpp,v 1.1 2007-03-09 17:08:56 janos Exp $
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

// from AlignTrTools
#include "AlignTrTools/IAlignSelTool.h"

// from AlignSolvTools
// #include "AlignSolvTools/IAlignSolvTool.h"

// local
#include "AlignAlgorithm.h"
#include "IAlignDetectorSelector.h" 

//-----------------------------------------------------------------------------
// Implementation file for class : AlignAlgorithm
//
// 2007-03-05 : Jan Amoraal
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AlignAlgorithm );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlignAlgorithm::AlignAlgorithm( const std::string& name,
                                ISvcLocator* pSvcLocator)
                                : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("AlignDetectorSelector", m_alignDetectorName  = "AlignOTDetector");
  declareProperty("TracksLocation", m_tracksLocation = "Rec/Track/Best");
  declareProperty("TracksSelector", m_trackSelectorName  = "AlignSelTool");
  declareProperty("ProjectorSelector", m_projSelectorName = "TrackProjectorSelector");
  //declareProperty("MatrixSolverTool", m_matrixSolverToolName = "AlignSolvTool");
} 
//=============================================================================
// Destructor
//=============================================================================
AlignAlgorithm::~AlignAlgorithm() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode AlignAlgorithm::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  /// Get tool to align detector
  m_alignDetector = tool<IAlignDetectorSelector>(m_alignDetectorName, "Detector", this);
  if (!m_alignDetector) {
    return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);
  }
  /// Get detector elements
  const Elements& elements = m_alignDetector->getDetectorElements();
  debug() << "Got " << elements.size() << " elements" << endmsg;
  /// Get number of elements
  unsigned int nElements = m_alignDetector->nElements();
  debug() << "Number of elements to align = " << nElements << endmsg;

  /// Get pivot points
  sc = getPivotPoints(elements, m_pivotPoints);

  /// Get alignment constants
  debug() << "==> Retrieving alignment paramterers" << endmsg;
  debug() << "==> Expect " << 9*elements.size() << " station alignment constants" << endmsg;
  sc = getAlignmentConstants(elements, m_initAlignConstants);
  if (sc.isFailure()) {
    return Error("==> Failed to retrieve initial alignment constants", StatusCode::FAILURE);
  }
  debug() << "==> Got " << m_initAlignConstants.size() << " station alignment constants" << endmsg;

  /// Get track selector tool
  m_trackSelector = tool<IAlignSelTool>(m_trackSelectorName, "Selector", this);
  if (!m_trackSelector) {
    return Error("==> Failed to retrieve track selector tool", StatusCode::FAILURE);
  }
  
  /// Get projector selector tool
  m_projSelector = tool<ITrackProjectorSelector>(m_projSelectorName, "Projector", this);
  if (!m_projSelector) {
    return Error("==> Failed to retrieve projector selector tool", StatusCode::FAILURE);
  }
  
  /// Get matrix solver tool
  //  m_matrixSolverTool = tool<IAlignSolvTool>(m_matrixSolverToolName, "MatrixSolver", this);
  // if (!m_matrixSolverTool) {
  //  return Error("==> Failed to retrieve matrix solver tool", StatusCode::FAILURE);
  //}
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignAlgorithm::execute() {

  debug() << "==> Execute" << endmsg;

  // Get tracks 
  Tracks* tracks = get<Tracks>(m_tracksLocation);
  debug() << "Number of tracks in container " + m_tracksLocation + " " << tracks->size() << endmsg;
  Tracks::const_iterator iT = tracks->begin();
  for ( ; iT != tracks->end(); ++iT) {
    // Check track
    // Maybe track selector tool should provide a container of "good" tracks?
    if ((m_trackSelector->selectTrack((*iT)))) {
      debug() << "==> Good track" << endmsg;
      // Get nodes. Need them for measurements, residuals and errors
      const Nodes& nodes = (*iT)->nodes();
      debug() << "==> Found " << nodes.size() << " nodes"<< endmsg;
      // Loop over nodes and get measurements, residuals and errors
      Nodes::const_iterator iN = nodes.begin();
      for ( ; iN != nodes.end(); ++iN) {
        /// Check if node has vaild pointer to a measurement
        if ((*iN)->hasMeasurement()) {
          debug() << "==> Node has valid pointer to a measurement" << endmsg;
          /// Get measurement
          const Measurement& meas = (*iN)->measurement();
          // Project measurement
          ITrackProjector* proj = m_projSelector->projector(meas);
          if (!proj) {
            debug() << "==> Could not get pojector for measurement!" << endmsg;
          } else {
            /// Get ID
            const LHCbID& id = meas.lhcbID();
            // Check if LHCb id is a valid id and get index associated to element
            typedef std::pair<bool, unsigned int> ValidIndex;
            ValidIndex elementIndex = m_alignDetector->index(id);
            bool valid = elementIndex.first;
            unsigned int index = elementIndex.second;
            if (valid) {
              debug() << "==> Index = " << index << endmsg;
              double res = (*iN)->residual();
              double err2 = (*iN)->errResidual2();
              debug() << "==> Pivot point " << m_pivotPoints[index] << endmsg;
              Derivatives der = proj->alignmentDerivatives(meas, m_pivotPoints[index]);
              m_mapDerivatives[unsigned(index)] += -1.0*((res*der)/err2);
              m_mapHMatrix[unsigned(index)] += Transpose(der)*der/err2;;
            }
          }
        }
      }
    } else {
      debug() << "==> Bad track!!" << endmsg;
    }
  }
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode AlignAlgorithm::finalize() {
  debug() << "==> Finalize" << endmsg;
  MapDerivatives::const_iterator iD = m_mapDerivatives.begin();
  for ( ; iD != m_mapDerivatives.end(); ++iD) {
    info() << "==> Element: " << iD->first << endmsg;
    info() << "==> rho = " << iD->second << endmsg;
  }  
  
  MapHMatrix::const_iterator iH = m_mapHMatrix.begin();
  for ( ; iH != m_mapHMatrix.end(); ++iH) {
    info() << "==> Element: " << iH->first << endmsg;
    info() << "==> H = " << iH->second << endmsg;
  }  

  /// Need to solve Ha=rho
  /// Update alignement constants
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
StatusCode AlignAlgorithm::getPivotPoints(const Elements& elements, PivotPoints& pivotPoints) const
{
  const AlignmentCondition *alignCond;
  pivotPoints.reserve(elements.size());
  Elements::const_iterator iE = elements.begin();
  for ( ; iE != elements.end(); ++iE) {
    debug() << (*iE)->name() << endmsg;
    alignCond = (*iE)->geometry()->alignmentCondition();
    if (!alignCond) {
      return StatusCode::FAILURE;
    }
    std::vector<double> pivotPoint = alignCond->paramVect<double>("pivotXYZ");
    debug() << "Pivot point = " << pivotPoint << endmsg;
    // Insert pivot point
    pivotPoints.push_back(Gaudi::XYZPoint(pivotPoint[0],pivotPoint[1],pivotPoint[2]));
  }
  return StatusCode::SUCCESS;
}

StatusCode AlignAlgorithm::getAlignmentConstants(const Elements& elements, AlignConstants& alignConstants) const
{
  const AlignmentCondition *alignCond;
  alignConstants.reserve(6*elements.size()); /// 9 constants * num elements
  Elements::const_iterator iE = elements.begin();
  for ( ; iE != elements.end(); ++iE) {
    debug() << (*iE)->name() << endmsg;
    alignCond = (*iE)->geometry()->alignmentCondition();
    if (!alignCond) {
      return StatusCode::FAILURE;
    }
    std::vector<double> translations = alignCond->paramVect<double>("dPosXYZ");
    std::vector<double> rotations = alignCond->paramVect<double>("dRotXYZ");
    /// Insert intitial constants (per element)
    alignConstants.insert(alignConstants.end(), translations.begin(), translations.end());
    alignConstants.insert(alignConstants.end(), rotations.begin(), rotations.end());
  }
  return StatusCode::SUCCESS;
}
