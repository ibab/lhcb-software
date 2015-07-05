// $Id: AlignAlgorithm.h,v 1.39 2010-04-17 16:21:39 wouter Exp $
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
#include "GaudiKernel/ToolHandle.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/Node.h"
#include "Event/RecVertex.h"
#include "Event/Particle.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackProjectorSelector.h"
#include "TrackInterfaces/ITrackProjector.h"
#include "TrackInterfaces/ITrackSelector.h"

// from AlignSolvTools
#include "AlignKernel/AlVec.h"
#include "AlignKernel/AlSymMat.h"

// from AlignmentInterfaces
#include "AlignmentInterfaces/IAlignSolvTool.h"
#include "AlignmentInterfaces/IAlignUpdateTool.h"
#include "AlignmentInterfaces/IWriteAlignmentConditionsTool.h"
//#include "AlignmentInterfaces/IWriteMultiAlignmentConditionsTool.h"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// from local
// @todo: this should be moved to the interfaces package
#include "IGetElementsToBeAligned.h"
#include "ITrackResidualTool.h"
#include "IVertexResidualTool.h"
#include "IAlignConstraintTool.h"
#include "AlignmentElement.h"
#include "AlElementHistos.h"

namespace Al {
  class Equations;
  class Residuals ;
}

/** @class AlignAlgorithm AlignAlgorithm.h
*
*
*  @author Jan Amoraal
*  @date   2007-03-05
*/

class IAlignWork;
class HistoUpdater;

class AlignAlgorithm : public GaudiHistoAlg, virtual public IIncidentListener {
public:
  /// Some handy typedefs
  typedef IGetElementsToBeAligned::Elements                        Elements;
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
  virtual StatusCode stop ();  ///<Transition executed upon the Stop command
  virtual StatusCode start();  ///<Transition executed upon the Start command
  /// Virtuals incident
  void handle(const Incident& incident);
  StatusCode queryInterface(const InterfaceID& id, void** ppI);

  /// Methods to call when an update is triggered
  void update();
  void update( const Al::Equations& equations ) ;
  const Al::Equations* equations() const { return m_equations ; }

  void reset();

  /** Method to get alignment constants, posXYZ and rotXYZ for a given set
  * of detector elements
  * @param elements flat vector of detector elements, i.e. std::vector<DetectorElements>
  * @param alignConstants reference to a flat vector of alignment constants, i.e. std::vector<double>
  */
  void getAlignmentConstants(const Elements& elements, AlignConstants& alignConstants) const;

protected:

  bool printDebug()   const {return msgLevel(MSG::DEBUG);};
  bool printVerbose() const {return msgLevel(MSG::VERBOSE);};
  bool accumulate( const Al::Residuals& residuals ) ;
  void resetHistos() ;

  typedef std::vector<LHCb::RecVertex> VertexContainer ;
  typedef std::vector<const LHCb::Track*> TrackContainer ;
  void selectVertexTracks( const LHCb::RecVertex& vertex, const TrackContainer& tracks,
        TrackContainer& tracksinvertex) const ;
  void removeVertexTracks( const LHCb::RecVertex& vertex, TrackContainer& tracks) const ;
  void removeParticleTracks( const LHCb::Particle& p, TrackContainer& tracks) const ;
  void splitVertex( const LHCb::RecVertex& vertex, const TrackContainer& tracks,
        VertexContainer& splitvertices) const  ;
  LHCb::RecVertex* cloneVertex( const LHCb::RecVertex& vertex, const TrackContainer& selectedtracks ) const ;
  bool testNodes( const LHCb::Track& track ) const ;

  HistoUpdater *m_HistoUpdater;

private:
  size_t                            m_iteration;                     ///< Iteration counter
  size_t                            m_nIterations;                   ///< Number of iterations
  size_t                            m_nTracks;                       ///< Number of tracks used
  size_t                            m_covFailure;                    ///< Number of covariance calculation failures
  IGetElementsToBeAligned*          m_align;                         ///< Pointer to tool to align detector
  std::string                       m_tracksLocation;                ///< Tracks for alignment
  std::string                       m_vertexLocation;                ///< Vertices for alignment
  std::string                       m_dimuonLocation;                ///< J/psi vertcies for alignment
  std::string                       m_particleLocation;              ///< particles with mass constraint for alignment
  std::string                       m_projSelectorName;              ///< Name of projector selector tool
  ITrackProjectorSelector*          m_projSelector;                  ///< Pointer to projector selector tool
  ToolHandle<Al::ITrackResidualTool>   m_trackresidualtool ;
  ToolHandle<Al::IVertexResidualTool>  m_vertexresidualtool ;
  ToolHandle<Al::IAlignUpdateTool>  m_updatetool ;
  ToolHandle<ITrackSelector>        m_vertextrackselector ;
  ToolHandle<IWriteAlignmentConditionsTool> m_xmlWritersTool ;
  //  std::vector<std::string>          m_xmlWriterNames ;
  //  std::vector<IWriteAlignmentConditionsTool*> m_xmlWriters ;
  std::string                       m_alignSummaryDataSvc ;
  std::string                       m_alignSummaryLocation ;
  bool                              m_fillHistos ;
  bool                              m_fireRunChange ;

  Al::Equations*                    m_equations;                     ///< Equations to solve
  bool                              m_correlation ;                  ///< Do we take into account correlations between residuals?
  bool                              m_updateInFinalize ;             ///< Call update from finalize
  double                            m_chi2Outlier ;                  ///< Chi2 threshold for outlier rejection
  bool                              m_usePreconditioning ;           ///< Precondition the system of equations before calling solver
  size_t                            m_minTracksPerVertex ;
  size_t                            m_maxTracksPerVertex ;
  std::string                       m_outputDataFileName ;
  std::vector<std::string>          m_inputDataFileNames ;
  unsigned int                      m_runnr;

  /// Monitoring
  // @todo: Move this to a monitoring tool
  std::vector<AlElementHistos*>     m_elemHistos ;
  bool                              m_resetHistos ; // reset histos on next event processing
  long long                         m_forcedInitTime ; // force the alignment geometry to initialize with this time (rather than first event)
  bool				    m_Online;
  IAlignWork*			    m_IAlwork;
};

#endif // TALIGNMENT_ALIGNALGORITHM_H
