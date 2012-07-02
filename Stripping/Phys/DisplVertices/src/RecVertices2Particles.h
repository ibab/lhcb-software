#ifndef RECVERTICES2PARTICLES_H 
#define RECVERTICES2PARTICLES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciHistoAlgorithm.h"

// from Gaudi
#include "GaudiKernel/VectorMap.h"

//To recreate track measurements
#include "TrackInterfaces/IMeasurementProvider.h"

// Detector description
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/ITransportSvc.h"
#include <VeloDet/DeVeloSensor.h>
#include <VeloDet/DeVeloRType.h>
#include "VeloDet/DeVelo.h"
#include "Kernel/IMatterVeto.h"   

#include "Kernel/IProtoParticleFilter.h"

/** @class RecVertices2Particles RecVertices2Particles.h
 *  @brief Turn a given container of RecVertex if they fulfill some criteria
 *  @author Neal Gauvin
 *  @date   12 janvier 2010
 */

class RecVertices2Particles : public DaVinciHistoAlgorithm {
public: 
  /// Standard constructor
  RecVertices2Particles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RecVertices2Particles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  const IVertexFit * m_vFit;
  IMeasurementProvider* m_measProvider;
  ITransportSvc * m_transSvc;
  IGeometryInfo* m_lhcbGeo;

  ///Get all RecVertex from RecVerticesLocation
  void GetRecVertices( LHCb::RecVertex::ConstVector & );
  ///Get the upstream PV
  const LHCb::RecVertex * GetUpstreamPV();
  void GetPVs(); ///< Get the PV candidates
  ///Get Nb of Velo tracks in event
  unsigned int GetNbVeloTracks();
  ///Turn a RecVertex into a Particle
  bool RecVertex2Particle( const LHCb::RecVertex*, 
			   int & ,
			   double r , LHCb::ProtoParticles& veloProtos );
  /// Has a RecVertex a backward track ?
  bool HasBackwardTracks( const LHCb::RecVertex* );
  /// Has a RecVertex a backward and a forward track ?
  bool HasBackAndForwardTracks( const LHCb::RecVertex* );
  /// Is a vertex isolated from other vertices ?
  bool IsIsolated( const LHCb::RecVertex*, LHCb::RecVertex::ConstVector & );
  ///Is a vertex too close to detector material ?
  bool IsAPointInDet( const LHCb::Particle &, int mode = 2 );
  bool TestMass( const LHCb::Particle * );  ///< Cut on the mass
  bool TestMass( LHCb::Particle & );  ///< Cut on the mass
  bool TestTrack( const LHCb::Track *  ); ///< Cut on the track quality
  void InitialiseGeoInfo();///< Store geometry infos
  bool IsInMaterialBoxLeft(const Gaudi::XYZPoint &);///<Point in material region in Left halfbox
  bool IsInMaterialBoxRight(const Gaudi::XYZPoint &);///<Point in material region in Right halfbox
  StatusCode UpdateBeamSpot(); ///< function to update the beam spot positions
  //Geometric tools
  double GetRFromBL( const Gaudi::XYZPoint& );
  void   GetSumPtNbGoodTrks( const LHCb::RecVertex *, double &, int & );

  std::string m_Prey;         ///< LHCb Name of the neutralino LSP
  LHCb::ParticleID m_PreyID;  ///< ID of this particle
  int    m_PreyPID;           ///< PID of the prey (for MC and Gen use)

  const double pi;

  int    m_nTracks;           ///< Min # of tracks at reconstructed vertex
  double m_PreyMinMass;       ///< Minimum reconstructed mass
  double m_PreyMaxMass;       ///< Maximum reconstructed mass
  double m_SumPt;             ///< Sumpt of all daughters tracks
  double m_RMin;              ///< Min dist to the z axis
  double m_RMax;              ///< Max dist to the z axis
  double m_TChi2;             ///< Max chi2/ndof of tracks to stay in Vtx.
  double m_Dist;              ///< Distance bet. RV to be said isolated
  const IMatterVeto* m_materialVeto;


  /***************************************************************//**
   * Remove vtx if in detector material ?
   * if = 0  : disabled
   * if = 5 :  if in home-made description of detector.
   ******************************************************************/
  int m_RemVtxFromDet;    
  bool        m_UsePartFromTES;
  /// Where RecVertices are stored on the TES
  std::vector<std::string> m_RVLocation;
  std::string m_Fitter;      ///< method for fitting the RecVertex
  /***************************************************************//**
   * the type of R cut to be applied 
   * ""                   : cut with respect to (0,0,z)
   * "FromUpstreamPV"     : cut with respect to the upstream PV (PV3D)
   * "FromBeamLine"       : cut with respect to given beam line
   * "FromBeamLineUpstreamPV" : cut with respect to given beam line
   *                            if no beam line is found, take the upPV.
   ******************************************************************/
  std::string m_RCut;         
  bool        m_FromBL;       ///< Set at initialisation acc. to m_RCut
  bool        m_FromUpPV;     ///< Set at initialisation acc. to m_RCut
  bool        m_FromBeamSpot; ///< Set at initialisation acc. to m_RCut
  std::string m_BLLoc;        ///< Location in TES of Beam line
  LHCb::Particle * m_BeamLine;
  double m_beamSpotX;
  double m_beamSpotY;
  std::vector<const LHCb::RecVertex*> PVs; ///< The PV candidates

  Gaudi::Transform3D m_toVeloLFrame; ///< to transform to local velo L frame
  Gaudi::Transform3D m_toVeloRFrame; ///< to transform to local velo R frame
  std::vector<Gaudi::XYZPoint > m_LeftSensorsCenter;
  std::vector<Gaudi::XYZPoint > m_RightSensorsCenter;
  /// set to true when geometry is already initialised
  bool        m_GeoInit;

  unsigned int m_PVnbtrks;    ///< Min nb of trks for upstream PV candidate
  double m_piMass;  ///< the pion mass
  double m_pt;      ///< default pt for default pions
  std::string m_veloProtoPartLocation;
  
  ///To sort the reconstructed vertices according to the z position
  class CondRVz{
  public:
    bool operator() ( const LHCb::RecVertex* first, 
		      const LHCb::RecVertex* second) const {
      return first->position().z() < second->position().z();
    }
  };

  //-------------------------------------------------------------------
  //This part defines the members used in the MakeParticle function
  /// Create Particle from ProtoParticle with best PID
  const LHCb::Particle * MakeParticle( const LHCb::ProtoParticle * );
  /// Map type that takes a particle type to a ProtoParticle filter
  typedef std::pair< const LHCb::ParticleProperty *, 
                     const IProtoParticleFilter* > ProtoPair;
  typedef std::vector < ProtoPair > ProtoMap;
  ProtoMap m_protoMap;
  
  class DLLPIDPair {
    
  private:
    std::string pid;
    double dll;
    
    DLLPIDPair() {};
    
  public:
    DLLPIDPair( std::string name, double val) :
      pid(name),
      dll(val) {};
    
    ~DLLPIDPair() {};
    std::string & GetPid(){ return pid; }
    const double & GetDll() const { return dll; }
    
    static bool cmp( const DLLPIDPair& one, const DLLPIDPair& two )  {
      return one.GetDll() > two.GetDll();
    }
  };
  
  typedef std::vector < DLLPIDPair > DLLPIDVector;   ///< DLL, PID pairs
  //-----------------------------------------

};

#endif // RECVERTICES2PARTICLES_H
