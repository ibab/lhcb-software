#ifndef HLT2PRESELDV_H 
#define HLT2PRESELDV_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

// from Gaudi
#include "GaudiKernel/VectorMap.h"

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

#include "VeloDet/DeVelo.h"

/** @class Hlt2PreSelDV Hlt2PreSelDV.h
 *  @brief Turn a given container of RecVertex if they fulfill some criteria
 *  @author Neal Gauvin
 *  @date   12 janvier 2010
 */

class Hlt2PreSelDV : public DVAlgorithm {
public: 
  /// Standard constructor
  Hlt2PreSelDV( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~Hlt2PreSelDV( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  const IVertexFit * m_vFit;
  IGeometryInfo* m_lhcbGeo;
  ITransportSvc * m_transSvc;

  ///Get all RecVertex from RecVerticesLocation
  void GetRecVertices( LHCb::RecVertex::ConstVector & );
  ///Get the upstream PV
  const LHCb::RecVertex * GetUpstreamPV();
  ///Turn a RecVertex into a Particle
  bool RecVertex2Particle( const LHCb::RecVertex*, 
			   const LHCb::Particle::ConstVector &, 
			   int & ,
			   double );
  /// Create a map between the Particles and the Velo Tracks
  void CreateMap( const LHCb::Particle::ConstVector & );
  /// Creates a pion with 400 MeV pt from track slopes.
  const LHCb::Particle* DefaultParticle( const LHCb::Track * p );
  /// Has a RecVertex a backward track ?
  bool HasBackwardTracks( const LHCb::RecVertex* );
  /// Has a RecVertex a backward and a forward track ?
  bool HasBackAndForwardTracks( const LHCb::RecVertex* );
  /// Has a candidate a daughter muon ?
  double HasMuons( const LHCb::Particle * );
  /// Is a vertex isolated from other vertices ?
  bool IsIsolated( const LHCb::RecVertex*, LHCb::RecVertex::ConstVector & );
  ///Is a vertex too close to detector material ?
  bool IsAPointInDet( const LHCb::Particle &, int mode = 2,
                      double range = 1*Gaudi::Units::mm );
  
  void InitialiseGeoInfo();///< Store geometry infos
  bool IsInMaterialBoxLeft(const Gaudi::XYZPoint &);///<Point in material region in Left halfbox
  bool IsInMaterialBoxRight(const Gaudi::XYZPoint &);///<Point in material region in Right halfbox

  bool TestMass( const LHCb::Particle * );  ///< Cut on the mass
  bool TestMass( LHCb::Particle & );  ///< Cut on the mass
  /// Cut on the RF-Foil position
  bool IsInRFFoil( const Gaudi::XYZPoint & );
  StatusCode SavePreysTuple( Tuple &, LHCb::Particle::ConstVector & );
  StatusCode fillHeader( Tuple & );
  StatusCode SaveGEC( Tuple &,  LHCb::Particle::ConstVector & );
  StatusCode SaveCaloInfos( Tuple & );
  StatusCode GetCaloInfos( std::string, double &, double & );

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

  const double pi ;

  int    m_nTracks ;          ///< Min # of tracks at reconstructed vertex
  unsigned int m_PVnbtrks;    ///< Min nb of trks for upstream PV candidate
  double m_PreyMinMass ;      ///< Minimum reconstructed mass
  double m_PreyMaxMass ;      ///< Maximum reconstructed mass
  double m_RMin;              ///< Min dist to the z axis
  double m_RMax;              ///< Max dist to the z axis
  double m_Dist;              ///< distance bet. RV to be said isolated
  /***************************************************************//**
   * Remove vtx if in detector material ?
   * if = 0  : disabled
   * if = 1  : remove reco vtx if in detector material
   * if = 2  : remove reco vtx if rad length from decay pos - DetDist 
   *           to decay pos + DetDist along momentum is > threshold
   * if = 3 : remove reco vtx if rad length along 
   *                             +- DetDist * PositionCovMatrix
   * if = 4 : 3 but range+3 if in RF foil.
   ******************************************************************/
  int m_RemVtxFromDet ;    
  double m_DetDist;           ///< Min distance to det material 
  ///Remove vtx if found in RF-Foil area, based on geometric cuts
  bool   m_RemFromRFFoil;
  bool   m_KeepLowestZ;       ///< keep the RV with the lowest Z (particle gun)
  bool   m_SaveTuple;         ///< Save candidate infos in a tuple
  bool   m_UseMap;            ///< Use a map to store Particle Track relation
  bool   m_MapCalled;         ///< has the map been already called in event ?
  /// Where RecVertices are stored on the TES
  std::vector<std::string> m_RVLocation ;
  std::string m_Fitter ;      ///< method for fitting the RecVertex
  /***************************************************************//**
   * the type of R cut to be applied 
   * ""                   : cut with respect to (0,0,z)
   * "FromUpstreamPV"     : cut with respect to the upstream PV (PV3D)
   * "FromBeamLine"       : cut with respect to given beam line
   ******************************************************************/
  std::string m_RCut;         
  std::string m_BLLoc;        ///< Location in TES of Beam line
  LHCb::Particle * m_BeamLine;

  Gaudi::Transform3D m_toVeloLFrame; ///< to transform to local velo L frame
  Gaudi::Transform3D m_toVeloRFrame; ///< to transform to local velo R frame
  std::vector<Gaudi::XYZPoint > m_LeftSensorsCenter;
  std::vector<Gaudi::XYZPoint > m_RightSensorsCenter;
  /// set to true when geometry is already initialised
  bool        m_GeoInit;

  GaudiUtils::VectorMap<int, const LHCb::Particle *> m_map;

  double m_piMass;  ///< the pion mass
  double m_pt;      ///< default pt for default pions

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

  ///To sort PVs with ascending z position 
  struct sortPVz {
    bool operator() ( const LHCb::RecVertex* first, 
                      const LHCb::RecVertex* second ) { 
      return first->position().z() < second->position().z();
    }
  } SortPVz;
  
};

#endif // RECVERTICES2PARTICLES_H
