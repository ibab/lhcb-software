// $Id: $
#ifndef DISPLVERTICES_H 
#define DISPLVERTICES_H 1

//#include <functional>

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

//To associate a particle to the true one.
#include "Kernel/Particle2MCLinker.h"

//To recreate track measurements
#include "TrackInterfaces/IMeasurementProvider.h"

//To use Wouter's vertex tool
#include "TrackInterfaces/ITrackVertexer.h"

//To use TrackMasterExtrapolator
#include "TrackInterfaces/ITrackExtrapolator.h"

//To refit particles
#include "Kernel/IParticle2State.h"
#include "TrackInterfaces/ITrackFitter.h"

//Hlt tools
#include "Kernel/ITriggerTisTos.h"

// Detector description
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/ITransportSvc.h"

/** @class DisplVertices DisplVertices.h
 *  
 *  @Version 6r3
 *  @author Neal Gauvin
 *  @date  30 june 2009
 */

//A list is a sequence container that support rapide insertion or deletion
//at any position but does not support random access.
typedef std::vector<LHCb::Particle> VecPart;
typedef std::vector<LHCb::Vertex> VecVert;

class DisplVertices : public DVAlgorithm {
public: 
  /// Standard constructor
  DisplVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DisplVertices( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  Particle2MCLinker* m_pLinker;
  IVertexFit *m_vFit;
  IMeasurementProvider* m_measProvider;
  ITrackVertexer * m_tFit; //Wouter's tool
  ITrackExtrapolator * m_extra;  //extrapolator
  IParticle2State* m_pToState;  //refit particle
  ITrackFitter* m_trackFit;     //refit particle
  ITrackFitter* m_trackPreFit;  //refit particle
  ITriggerTisTos* m_tisTos;
  ITransportSvc * m_transSvc;
  IGeometryInfo* m_lhcbGeo;

  //Studies
  void StudyActivity();
  void StudyDistanceBet2Interactions();
  void StudyDiffGenMC();
  void StudyPV();
  void StudyHltEfficiency( const Gaudi::XYZPoint & );
  void StudyPreyIP( const LHCb::Particle *, const LHCb::RecVertex * );
  void StudyDispersion( const LHCb::Particle * );
  void StudyDispersion( const LHCb::Particle *, const LHCb::Particle * );
  void StudyDgtsIP( const LHCb::Particle * );
  void StudyDgtsIP( const LHCb::RecVertex * );
  void StudyEoverNbTrk();
  void StudyPreyComposition( const LHCb::Particle *, std::string );
  void StudyParticleGun( LHCb::Particle::ConstVector & );

  //HepMC and MC functions
  StatusCode GetHepMCInfos();
  void GetHepMCStable( HepMC::GenParticle *, int &, HepMC::ThreeVector* );
  void GetHepMCStable( HepMC::GenParticle *, int &, HepMC::GenVertex* );
  std::string Print( HepMC::ThreeVector );
  std::string Print( HepMC::FourVector );

  StatusCode GetMCInfos();
  bool GetMCPrey( const LHCb::Particle * );
  void GetMCStable( const LHCb::MCVertex *, std::string );
  void GetMCStable( const LHCb::MCVertex * );
  void GetMCStable( const LHCb::MCParticle *, const Gaudi::XYZPoint &, 
		    LHCb::MCParticle::ConstVector & );
  void GetMCStable( const LHCb::MCParticle *, const Gaudi::XYZPoint &, 
		    unsigned int & );
  void GetMCStable( const LHCb::MCParticle *, unsigned int & );
  void GetMCStable( const LHCb::MCVertex *, int &, Gaudi::LorentzVector & );
  void SaveGenPartinTuple( const HepMC::GenEvent* );
  void SaveMCPartinTuple( LHCb::MCParticles * );
  StatusCode SaveGEC( Tuple &,  LHCb::Particle::ConstVector & ); 

  //questions
  double WhichMCPrey( const LHCb::Particle * );
  double IsitFromaPrey( const LHCb::MCParticle *, const LHCb::MCVertex * );
  bool IsitFromaPrey( const LHCb::MCParticle *);
  bool IsitFromaPrey( HepMC::GenParticle * );
  bool IsNeutrino( const LHCb::MCParticle *);
  bool IsNeutrino( HepMC::GenParticle * );
  bool IsQuark( HepMC::GenParticle * );
  bool IsAPointInDet( const LHCb::Particle *, int mode = 2,
                      double range = 1*Gaudi::Units::mm );
  bool IsInRFFoil( const Gaudi::XYZPoint & );
  bool HasBackwardTracks( const LHCb::Particle * );
  bool HasBackwardTracks( const LHCb::RecVertex * );
  double HasMuons( const LHCb::Particle * );
  const LHCb::MCParticle * WhichMother( const LHCb::Particle * );
  const LHCb::MCParticle * WhichMother( const LHCb::MCParticle * );

  //Geometric tools
  double Mult( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  double Mult( const Gaudi::LorentzVector &, const Gaudi::LorentzVector & );
  Gaudi::XYZPoint Plus( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  Gaudi::XYZPoint Minus( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  double Norm( const Gaudi::LorentzVector &);
  double Norm( const Gaudi::XYZPoint &);
  double Norm( const Gaudi::XYZVector &);
  Gaudi::XYZPoint Normed( const Gaudi::LorentzVector &, double range = 1 );
  double VertDistance( const Gaudi::XYZPoint &, const Gaudi::XYZPoint &);
  double VertDistance( HepMC::ThreeVector &,  HepMC::ThreeVector & );
  double VertDistance( HepMC::GenVertex*, HepMC::GenVertex* );
  double VertDistance( HepMC::ThreeVector *, LHCb::Vertex * );
  double VertDistance( HepMC::ThreeVector *, HepMC::GenVertex* );
  double VertDistance( HepMC::ThreeVector *, const LHCb::RecVertex * ) const;
  double VertDistance( HepMC::ThreeVector *, const LHCb::RecVertex & ) const;
  double VertDistance( HepMC::ThreeVector &, LHCb::RecVertex & );
  double VertDistance( HepMC::ThreeVector *, HepMC::ThreeVector & );
  double VertDistance( HepMC::ThreeVector & ); //return distance to ref frame.
  double Angle( const Gaudi::LorentzVector &, const Gaudi::LorentzVector & );
  double Angle( const Gaudi::XYZVector &, const Gaudi::XYZVector & );
  double ImpactParam( const LHCb::MCParticle *, const LHCb::MCVertex * );
  double ImpactParam( const Gaudi::XYZPoint &, const Gaudi::LorentzVector &, 
		      const Gaudi::XYZPoint & );
  double ImpactParam( const Gaudi::XYZPoint &, const Gaudi::XYZVector&, 
		      const Gaudi::XYZPoint &);
  void Resolution( const Gaudi::XYZPoint & );
  void Resolution( );

  double GetSumPt( const LHCb::Particle * );
  double GetRecMass( const LHCb::Particle::ConstVector & );
  double GetRecMass( const LHCb::Particle::ConstVector &, double & );
  double GetPt( const LHCb::RecVertex* RV );
  double GetPt( const Gaudi::LorentzVector &, const Gaudi::LorentzVector & );
  double GetDeltaR( const LHCb::Particle *, const LHCb::Particle * );
  double GetR( const Gaudi::XYZPoint & );
  double GetMCMass( LHCb::MCParticle::ConstVector & );
  double GetRFromBL( const Gaudi::XYZPoint& );
  
  void GetUpstreamPV();
  void GetPartsFromRecVtx(const LHCb::RecVertex*, 
			  const LHCb::Particle::ConstVector &, 
			  LHCb::Particle::ConstVector & );
  void GetCaloInfos( std::string, Tuple &, double En = 0., double Et = 0.);
  void GetCaloInfos( std::string, double En = 0., double Et = 0.);
  StatusCode SaveCaloInfos( Tuple & );
  StatusCode SaveTrigInfinTuple( Tuple & );
  StatusCode fillHeader( Tuple & );
  StatusCode ReconstructMother( LHCb::Particle::ConstVector & );

  /*****************************************************************
   * the type of R cut to be applied 
   * ""                   : cut with respect to (0,0,z)
   * "FromUpstreamPV"     : cut with respect to the upstream PV (PV3D)
   * "FromBeamLine"       : cut with respect to given beam line
   ******************************************************************/
  std::string m_RCut;         
  std::string m_BLLoc;        ///< Location in TES of Beam line
  LHCb::Particle * m_BeamLine;
  bool   m_ok ;               // Ok if at least one 95% pure khi is reco.
  bool   m_SaveTuple;         ///<Save prey properties in a tuple 
  bool   m_SaveTrigInfos ;    ///<Save trigger infos in a tuple
  bool   m_SaveonTES;         ///<Save Event on TES if fires
  int    m_nEvents ;          // number of events
  const double pi ;
  double m_purity ;           ///< Purity of reconstructed neuts
  ///Proportion of preys in different purity areas
  std::vector<double> m_purities;
  double m_maxunpure ;
  double m_minmixed ;
  double m_maxmixed ;
  double m_a1 ;               //pure prey
  double m_a2 ;               //mixed prey
  double m_a3 ;               //totally unpure prey
  double m_nbassfromaPrey;    // nb of true MC tracks in neut
  int    m_nPreys ;           // nb of reconstructed preys.
  int    m_nMother ;           // nb of reconstructed higgses
  int    m_RecbleMCPrey ;
  // cuts
  double m_PreyMinMass ;      ///< Minimum reconstructed mass
  double m_PreyMaxMass ;      ///< Maximum reconstructed mass
  double m_SumPt ;            ///< Sumpt of all daughters tracks
  double m_DistMax;           ///< Max distance of the vertices 
  double m_RMin;              ///< Min dist to the z axis
  double m_RMax;              ///< Max dist to the z axis
  double m_MaxChi2OvNDoF;     ///< Max chi2 of a vertex
  double m_PurityMin;         ///< Min purity requested, MC set on
  double m_MuonpT;            ///< Muon with a pT of min m_MuonpT
  double m_DocaMax;           ///< Max distance of closest approach
  unsigned int m_NbCands;     ///< Min nb of desired candidates
  int    m_nTracks ;          ///< Min # of tracks at reconstructed vertex
  /*****************************************************************
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
  //Remove vtx if found in RF-Foil area, based on geometric cuts
  bool   m_RemFromRFFoil;

  Gaudi::Transform3D m_toVeloLFrame; ///< to transform to local velo L frame
  Gaudi::Transform3D m_toVeloRFrame; ///< to transform to local velo R frame

  bool   m_MC;                ///< Retrieve MC infos or not
  std::string m_HepMC;        ///< Retrieve HepMC infos or not
  std::vector<Gaudi::XYZPoint> m_MCSVPos; //on array of ptr on MCSV
  std::vector<Gaudi::XYZPoint> m_MCPos; //on array of ptr on MCPrey
  const LHCb::RecVertex * PV; //The Primary Vertex !
  const LHCb::MCVertex* MCPV; //The MC Primary Vertex !

  std::string m_Prey ;        ///< LHCb Name of the prey
  LHCb::ParticleID m_PreyID;  // PDG ID of this particle
  int    m_PreyPID ;          ///< PID of the prey (for MC and Gen use)
  std::string m_MotherPrey ;  ///< LHCb Name of the mother of the prey
  LHCb::ParticleID m_MotherPreyID;  // PDG ID of this particle
  bool   m_IsPreyFromMother ;       ///< Is the Prey from the given Mother ?

  //Dummy things for HepMC
  HepMC::ThreeVector * kdec1;           // the neutralino MC decay vertex
  HepMC::ThreeVector * kdec2;           // the neutralino MC decay vertex
  HepMC::FourVector dumV1;       // dummy lorentz vector
  HepMC::FourVector dumV2;       // dummy lorentz vector

  struct sortPVdz {
    double refz; 
    bool operator() ( const LHCb::RecVertex* first, 
		      const LHCb::RecVertex* second ) { 
      
      return std::abs( first->position().z() - refz ) < 
	std::abs( second->position().z() - refz );
    }
  } SortPVdz;
  ///To sort PVs with ascending z position 
  struct sortPVz {
    bool operator() ( const LHCb::RecVertex* first, 
		      const LHCb::RecVertex* second ) { 
      return first->position().z() < second->position().z();
    }
  } SortPVz;

};

#endif // DISPLVERTICES_H
