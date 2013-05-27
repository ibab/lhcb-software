#ifndef VPV0S_H 
#define VPV0S_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "VPDet/DeVP.h"
#include "TrackInterfaces/IVPClusterPosition.h"
#include "VPDet/DeVP.h"
#include "Event/VPLiteCluster.h"
#include "MCInterfaces/ILHCbIDsToMCHits.h"
#include "GaudiKernel/ToolHandle.h"

class ITrajPoca ;
class IMagneticFieldSvc ;
class ITrackVertexer ;
class ITrackExtrapolator ;
class ITrackInterpolator ;

#include "Event/TwoProngVertex.h"
namespace LHCb {
  class ParticleProperty ;
}

/** @class VPV0s VPV0s.h
 *  
 *   Algorithm to produce an NTuple to check Pat tracks.
 *  @author Olivier Callot
 *  @date   2006-05-11
 *  @update for A-Team framework 2007-08-20
 */


  class VPV0s : public GaudiTupleAlg {
  public: 
    /// Standard constructor
    VPV0s( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VPV0s( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    ILHCbIDsToMCHits* linkTool() const;
    
  protected:
      void constrainToVertex(const Gaudi::XYZPoint& pos,
			 const Gaudi::LorentzVector& p4,
			 const Gaudi::SymMatrix7x7& cov7x7,
			 const LHCb::RecVertex& pv,
			 double& chi2,
			 double& decaylength,
			 double& decaylengtherr) const ;
  
  bool hasV0Topology(LHCb::TwoProngVertex& vertex,
		     const LHCb::RecVertices& pvs) const ;
  private:
    std::string m_tupleName;
    std::string m_v0ContainerName;
    DeVP* m_vP;
    IVPClusterPosition* m_positiontool ;
    ILHCbIDsToMCHits* m_linkTool;
    LHCb::VPLiteCluster::VPLiteClusters* m_clusters;
    IMagneticFieldSvc* m_magfieldsvc ;
    ITrajPoca* m_pocatool ;
    ToolHandle<ITrackExtrapolator> m_extrapolator ;
    ToolHandle<ITrackInterpolator> m_interpolator ;
    ITrackVertexer* m_vertexer ;
    const LHCb::ParticleProperty* m_ksProperty ;
    const LHCb::ParticleProperty* m_lambdaProperty ;
    const LHCb::ParticleProperty* m_pionProperty ;
    const LHCb::ParticleProperty* m_protonProperty ;
    
    unsigned int m_maxNumCommonHits ;
    double m_distanceCutLong ;
    double m_deltaZCut ;
    double m_maxChi2V0Vertex ;
    double m_maxChi2PVConstraint ;
    double m_minDecayLengthSignificance ;
    bool   m_correctBFlight ;
    double m_sigmaBFlightX ;
    double m_sigmaBFlightY ;
    double m_sigmaBFlightZ ;
    double m_ksmasscut ;
    double m_lambdamasscut ;
    bool m_useExtrapolator ;
    bool m_excludePVTracks ;
    bool m_rejectUpstreamHits ;
    bool m_addExtraInfo ;
  };


inline ILHCbIDsToMCHits* VPV0s::linkTool() const{
  return m_linkTool;
}

#endif // VPV0S_H
