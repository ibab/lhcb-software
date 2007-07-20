// $Id: AlignAlgorithm.h,v 1.7 2007-07-20 17:24:33 janos Exp $
#ifndef TALIGNMENT_ALIGNALGORITHM_H 
#define TALIGNMENT_ALIGNALGORITHM_H 1

// Include files
// from std
#include <string>
#include <vector>
#include <utility>
#include <map>

// from Gaudi
//#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/Point3DTypes.h"

// from AlignSolvTools
#include "SolvKernel/AlVec.h"
#include "SolvKernel/AlSymMat.h"

// forward declarations
class DetectorElement;
//class IAlignSelTool;
class ITrackProjectorSelector;
class IAlignDetectorSelector;
class IAlignSolvTool;

namespace LHCb 
{
  class Track;
  class Node;
};

namespace AIDA 
{
  class IHistogram2D;
};

/** @class AlignAlgorithm AlignAlgorithm.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-03-05
 */

class AlignAlgorithm : public GaudiHistoAlg, virtual public IIncidentListener {

public:
  /// Some handy typedefs
  typedef std::vector<DetectorElement*> Elements;
  typedef std::vector<Gaudi::XYZPoint> PivotPoints;
  typedef std::vector<double> AlignConstants;
  typedef std::vector<LHCb::Node*> Nodes;
  typedef Gaudi::Matrix1x6 Derivatives;
  typedef Gaudi::Matrix6x6 HMatrix;
  typedef std::map<unsigned int, std::pair<HMatrix, Derivatives> > Equations; 

  /// Standard constructor
  AlignAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~AlignAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

  /// Virtuals incident
  void handle(const Incident& incident);
  StatusCode queryInterface(const InterfaceID& id, void** ppI);
    
  /// Method to call when an update is triggered
  void update();
  void reset();

  /** Method to get pivot points, pivotXYZ, for a given set of detector elements
  * @param flat vector of detector elements, i.e. std::vector<DetectorElement>
  * @param reference to a flat vector of pivot points, i.e. std::vector<XYZPoint>
  * @return StatusCode
  */
  StatusCode getPivotPoints(const Elements& elements, PivotPoints& pivotPoints) const;

  /** Method to get alignment constants, posXYZ and rotXYZ for a given set 
  * of detector elements
  * @param elements flat vector of detector elements, i.e. std::vector<DetectorElements>
  * @param alignConstants reference to a flat vector of alignment constants, i.e. std::vector<double>
  * @return StatusCode
  */
  StatusCode getAlignmentConstants(const Elements& elements, AlignConstants& alignConstants) const;

  /** Method to put alignment constants
  * @param elements flat vector of detector elements
  * @param alignConstants Alignment vector of parameters
  * @return StatusCode
  */
  StatusCode putAlignmentConstants(const Elements& elements, const AlVec& alignConstants) const;
 
protected:

private:
  /// handy typedefs
  typedef Elements::const_iterator ElemIter;

  bool printDebug() const {return msgLevel(MSG::DEBUG);};

  size_t                   m_iteration;            ///< Iteration counter
  size_t                   m_nIterations;          ///< Number of iterations
  Elements                 m_elements;             ///< Detector elements
  size_t                   m_nDetElements;         ///< Number of detector elements
  size_t                   m_nAlignConstants;      ///< Number of align parameters
  size_t                   m_nTracks;              ///< Number of tracks
  std::vector<double>      m_initAlignConstants;   ///< Initial alignment constants
  PivotPoints              m_pivotPoints;          ///< pivot points
  std::string              m_alignDetectorName;    ///< name of tool to align detector
  IAlignDetectorSelector*  m_alignDetector;        ///< Pointer to tool to align detector
  std::string              m_tracksLocation;       ///< Tracks location for alignment
  std::string              m_projSelectorName;     ///< Name of projector selector tool
  ITrackProjectorSelector* m_projSelector;         ///< Pointer to projector selector tool
  std::string              m_matrixSolverToolName; ///< Name of linear algebra solver tool
  IAlignSolvTool*          m_matrixSolverTool;     ///< Pointer to linear algebra solver tool
  Equations                m_equations;            ///< Equations to solve 
  AlVec                    m_derivatives;          ///< Alignment vector of derivatives
  AlSymMat                 m_hMatrix;              ///< Alignment H matrix

  /// Monitoring
  /// Residules (id =1000 + n)
  std::map<unsigned int, IHistogram2D*> resHistos;
};

#endif // TALIGNMENT_ALIGNALGORITHM_H
