#ifndef RECVERTICES2PARTICLES_H 
#define RECVERTICES2PARTICLES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

//To recreate track measurements
#include "TrackInterfaces/IMeasurementProvider.h"

// Detector description
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/ITransportSvc.h"

/** @class RecVertices2Particles RecVertices2Particles.h
 *  
 *  @Version 6r0
 *  @author Neal Gauvin
 *  @date   12 janvier 2010
 */

class RecVertices2Particles : public DVAlgorithm {
public: 
  /// Standard constructor
  RecVertices2Particles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RecVertices2Particles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  IVertexFit * m_vFit;
  IMeasurementProvider* m_measProvider;
  ITransportSvc * m_transSvc;
  IGeometryInfo* m_lhcbGeo;

  ///Get all RecVertex from RecVerticesLocatio
  void GetRecVertices( LHCb::RecVertex::ConstVector & );
  ///Get the upstream PV
  const LHCb::RecVertex * GetUpstreamPV();
  ///Turn a RecVertex into a Particle
  bool RecVertex2Particle( const LHCb::RecVertex*, 
			   LHCb::Particle::ConstVector &, 
			   LHCb::Particle::ConstVector & );
  /// Has a RecVertex a backward track ?
  bool HasBackwardTracks( const LHCb::RecVertex* );
  /// Is a vertex isolated from other vertices ?
  bool IsIsolated( const LHCb::RecVertex*, LHCb::RecVertex::ConstVector & );
  ///Is a vertex too close to detector material ?
  bool RemVtxFromDet( LHCb::Particle & );
  ///Cut on the mass
  bool TestMass( const LHCb::Particle * );
  bool TestMass( LHCb::Particle & );
  StatusCode SavePreysTuple( Tuple &, LHCb::Particle::ConstVector & );
  StatusCode fillHeader( Tuple & );

  //Geometric tools
  double RFromBL( const Gaudi::XYZPoint& );
  double GetSumPt( const LHCb::Particle * );
  double VertDistance( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  double Angle( const Gaudi::LorentzVector &, const Gaudi::LorentzVector & );
  double Angle( const Gaudi::XYZVector &, const Gaudi::XYZVector & );
  Gaudi::XYZPoint Plus( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  Gaudi::XYZPoint Minus( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  double Mult( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  double Mult( const Gaudi::LorentzVector &, const Gaudi::LorentzVector & );
  double Norm( const Gaudi::LorentzVector &);
  double Norm( const Gaudi::XYZPoint &);
  double Norm( const Gaudi::XYZVector &);
  Gaudi::XYZPoint Normed( const Gaudi::LorentzVector &, double range = 1 );

  std::string m_Prey ;        ///< LHCb Name of the neutralino LSP
  LHCb::ParticleID m_PreyID;  ///< ID of this particle
  int    m_PreyPID ;          ///< PID of the prey (for MC and Gen use)

  int    m_nEvents ;          ///< event nb being processed
  const double pi ;

  int    m_nTracks ;          ///< Min # of tracks at reconstructed vertex
  double m_PreyMinMass ;      ///< Minimum reconstructed mass
  double m_PreyMaxMass ;      ///< Maximum reconstructed mass
  double m_RMin;              ///< Min dist to the z axis
  double m_RMax;              ///< Max dist to the z axis
  double m_Dist;              ///< distance bet. RV to be said isolated
  /*****************************************************************
   * Remove vtx if in detector material ?
   * if = 0  : disabled
   * if < 0  : remove reco vtx if in detector material
   * if > 0  : remove reco vtx if rad length from decay pos - m_RemVtxFromDet 
   *           to decay pos + m_RemVtxFromDet along z is > threshold
   ******************************************************************/
  double m_RemVtxFromDet ;    ///< 
  bool   m_KeepLowestZ ;      ///< keep the RV with the lowest Z (particle gun)
  bool   m_SaveTuple ;        ///< Save candidate infos in a tuple
  /// Where RecVertices are stored on the TES
  std::vector<std::string> m_RVLocation ;
  std::string m_Fitter ;      ///< method for fitting the RecVertex
  /*****************************************************************
   * the type of R cut to be applied 
   * ""                   : cut with respect to (0,0,z)
   * "FromUpstreamPV"     : cut with respect to the upstream PV (PV3D)
   * "FromBeamLine"       : cut with respect to given beam line
   ******************************************************************/
  std::string m_RCut;         
  std::string m_BLLoc;        ///< Location in TES of Beam line
  LHCb::Particle * m_BeamLine;

  ///To sort the reconstructed vertices according to the z position
  class CondRVz{
  public:
    bool operator() ( const LHCb::RecVertex* first, 
		      const LHCb::RecVertex* second) const {
      return first->position().z() < second->position().z();
    }
  };
  
  //Use for study and debugging purpose
  void PrintTrackandParticles();
  void StudyPreyComposition( const LHCb::Particle * );

};

#endif // RECVERTICES2PARTICLES_H
