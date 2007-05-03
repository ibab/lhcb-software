// $Id: AlignAlgorithm.h,v 1.5 2007-05-03 07:57:11 janos Exp $
#ifndef ALIGNALGORITHM_H 
#define ALIGNALGORITHM_H 1

// Include files
// from std
#include <string>
#include <vector>
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/Point3DTypes.h"

// from AlignSolvTools
#include "AlignSolvTools/AlVec.h"
#include "AlignSolvTools/AlSymMat.h"

// forward declarations
class DetectorElement;
class IAlignSelTool;
class ITrackProjectorSelector;
class IAlignDetectorSelector;
class IAlignSolvTool;
namespace LHCb 
{
  class Track;
  class Node;
};

/** @class AlignAlgorithm AlignAlgorithm.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-03-05
 */

class AlignAlgorithm : public GaudiAlgorithm {

public:
  /// Some handy typedefs
  typedef std::vector<DetectorElement*> Elements;
  typedef std::vector<Gaudi::XYZPoint> PivotPoints;
  typedef std::vector<double> AlignConstants;
  typedef std::vector<LHCb::Node*> Nodes;
  typedef Gaudi::Matrix1x6 Derivatives;
  typedef Gaudi::Matrix6x6 HMatrix;
  typedef std::map<unsigned int, Derivatives> MapDerivatives;
  typedef std::map<unsigned int, HMatrix> MapHMatrix;
 
  /// Standard constructor
  AlignAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~AlignAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /** Method to get pivot points, pivotXYZ, for a given set of detector elements
  * @param flat vector of detector elements, i.e. std::vector<DetectorElement>
  * @param reference to a flat vector of pivot points, i.e. std::vector<XYZPoint>
  */
  StatusCode getPivotPoints(const Elements& elements, PivotPoints& pivotPoints) const;

  /** Method to get alignment constants, posXYZ and rotXYZ for a given set 
  * of detector elements
  * @param flat vector of detector elements, i.e. std::vector<DetectorElements>
  * @param reference to a flat vector of alignment constants, i.e. std::vector<double>
  * @returns StatusCode
  */
  StatusCode getAlignmentConstants(const Elements& elements, AlignConstants& alignConstants) const;

  /** Method to put alignment constants
  * @param alignment vector of parameters
  */
  StatusCode putAlignmentConstants(const Elements& elements, const AlVec& alignConstants) const;
 
protected:

private:
  Elements m_elements;                        ///< Detector elements
  unsigned int m_nDetElements;                       ///< Number of detector elements
  unsigned int m_nAlignConstants;                    ///< Number of align parameters
  unsigned int m_nTracks;                            ///< Number of tracks
  unsigned int m_nGoodTracks;                        ///< Number of good tracks
  std::vector<double> m_initAlignConstants;          ///< Initial alignment constants
  PivotPoints m_pivotPoints;                         ///< pivot points
  std::string m_alignDetectorName;                   ///< name of tool to align detector
  IAlignDetectorSelector* m_alignDetector;           ///< Pointer to tool to align detector
  std::string m_tracksLocation;                      ///< Tracks location for alignment
  std::string m_trackSelectorName;                   ///< Name of track selector for alignment
  IAlignSelTool* m_trackSelector;                    ///< Pointer to track selector tool for alignment
  std::string m_projSelectorName;                    ///< Name of projector selector tool
  ITrackProjectorSelector* m_projSelector;           ///< Pointer to projector selector tool
  std::string m_matrixSolverToolName;                ///< Name of linear algebra solver tool
  IAlignSolvTool* m_matrixSolverTool;                ///< Pointer to linear algebra solver tool
  MapDerivatives m_mapDerivatives;                   ///< Map of derivatives to detector element index
  MapHMatrix m_mapHMatrix;                           ///< Map of matrices to detector element index
  AlVec m_derivatives;                               ///< Alignment vector of derivatives
  AlSymMat m_hMatrix;                                ///< Alignment H matrix
};
#endif // ALIGNALGORITHM_H
