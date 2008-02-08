// $Id: AlignAlgorithm.h,v 1.22 2008-02-08 10:02:12 wouter Exp $
#ifndef TALIGNMENT_ALIGNALGORITHM_H
#define TALIGNMENT_ALIGNALGORITHM_H 1

// Include files
// from std
#include <string>
#include <vector>
#include <utility>
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/Point3DTypes.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/Node.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackProjectorSelector.h"
#include "TrackInterfaces/ITrackProjector.h"

// from AlignSolvTools
#include "SolvKernel/AlVec.h"
#include "SolvKernel/AlSymMat.h"

// from AlignmentInterfaces
#include "AlignmentInterfaces/IAlignSolvTool.h"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// from local
// @todo: this should be moved to the interfaces package
#include "IGetElementsToBeAligned.h"
#include "AlignmentElement.h"

namespace {
  class Equations;
}

/** @class AlignAlgorithm AlignAlgorithm.h
*
*
*  @author Jan Amoraal
*  @date   2007-03-05
*/

class AlignAlgorithm : public GaudiHistoAlg, virtual public IIncidentListener {

public:
  /// Some handy typedefs
  typedef IGetElementsToBeAligned::Elements                        Elements;
  typedef IGetElementsToBeAligned::ElementRange                    ElementRange ;
  typedef std::vector<double>                                      AlignConstants;
  typedef std::vector<LHCb::Node*>                                 Nodes;
  typedef Gaudi::Matrix1x6                                         Derivatives;
  typedef Gaudi::Matrix6x6                                         HMatrix;
  enum { CanonicalConstraintOff=0, CanonicalConstraintOn, CanonicalConstraintAuto} CanonicalConstraintStrategy ;

  /// Standard constructor
  AlignAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~AlignAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

  /// Virtuals incident
  void handle(const Incident& incident);
  StatusCode queryInterface(const InterfaceID& id, void** ppI);

  /// Methods to call when an update is triggered
  void update();
  void reset();

  /** Add canonical constraints. Return the number of added constraints. */
  size_t addCanonicalConstraints(AlVec& dChi2dAlpha, AlSymMat& d2Chi2dAlpha2,
				 std::vector<bool>& dofmask, std::ostream& logmessage) const ;
  void printCanonicalConstraints(const AlVec& parameters, const AlSymMat& covariance,
				 size_t numConstraints, std::ostream& logmessage) const ;
  void preCondition(AlVec& dChi2dAlpha, AlSymMat& d2Chi2dAlpha2,AlVec& scale, const std::vector<int>& offsets) const ;
  void postCondition(AlVec& dChi2dAlpha, AlSymMat& d2Chi2dAlpha2, const AlVec& scale) const ;
  
  /** Method to get alignment constants, posXYZ and rotXYZ for a given set
  * of detector elements
  * @param elements flat vector of detector elements, i.e. std::vector<DetectorElements>
  * @param alignConstants reference to a flat vector of alignment constants, i.e. std::vector<double>
  */
  void getAlignmentConstants(const ElementRange& rangeElements, AlignConstants& alignConstants) const;

protected:
  
private:
  bool printDebug() const {return msgLevel(MSG::DEBUG);};
  bool printVerbose() const {return msgLevel(MSG::VERBOSE);};

  size_t                            m_iteration;            ///< Iteration counter
  size_t                            m_nIterations;          ///< Number of iterations
  ElementRange                      m_rangeElements;        ///< Detector elements
  std::vector<double>               m_initAlignConstants;   ///< Initial alignment constants
  IGetElementsToBeAligned*          m_align;                ///< Pointer to tool to align detector
  std::string                       m_tracksLocation;       ///< Tracks location for alignment
  std::string                       m_projSelectorName;     ///< Name of projector selector tool
  ITrackProjectorSelector*          m_projSelector;         ///< Pointer to projector selector tool
  std::string                       m_matrixSolverToolName; ///< Name of linear algebra solver tool
  IAlignSolvTool*                   m_matrixSolverTool;     ///< Pointer to linear algebra solver tool
  Equations*                        m_equations;            ///< Equations to solve
  std::vector<std::vector<double> > m_constraints;
  bool                              m_correlation ;         ///< do we take into account correlations between residuals?
  bool                              m_updateInFinalize ;    ///< Call update from finalize
  int                               m_canonicalConstraintStrategy ; ///< Constrain global dofs
  bool                              m_constrainZShearings;  ///< Constrain z-shearings
  bool                              m_useWeightedAverageConstraint ; ///< Flag to turn on weighting in constraint averages
  size_t                            m_minNumberOfHits ;     ///< Minimum number of hits for an Alignable to be aligned
  bool                              m_usePreconditioning ;   ///< Precondition the system of equations before calling solver
  bool                              m_fillCorrelationHistos; ///< Flag to turn on filling of residual correlation histos

  /// Monitoring
  // @todo: Move this to a monitoring tool
  std::map<unsigned int, IHistogram2D*>                          m_resHistos;
  std::map<unsigned int, IHistogram2D*>                          m_pullHistos;
  std::map<unsigned int, IHistogram1D*>                          m_nHitsHistos;
  std::map<unsigned int, IHistogram2D*>                          m_autoCorrHistos;
  std::map<std::pair<unsigned int, unsigned int>, IHistogram2D*> m_corrHistos;
  IProfile1D*                                                    m_totChi2vsIterHisto;
  IProfile1D*                                                    m_norTotChi2vsIterHisto;
  IProfile1D*                                                    m_dChi2AlignvsIterHisto;
};

#endif // TALIGNMENT_ALIGNALGORITHM_H
