// $Id: $
#ifndef PARTICLEFLOW4JETS_H 
#define PARTICLEFLOW4JETS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// interfaces
#include "TrackInterfaces/ITrackSelector.h"
#include "TrackInterfaces/IHitExpectation.h"
#include "Kernel/IParticle2State.h"
// Event
#include "Event/State.h"
#include "Event/Track.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
#include "Kernel/IProtoParticleFilter.h"
#include "GaudiKernel/VectorMap.h"
#include "GaudiAlg/ITupleTool.h"

#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Relations/RelationWeighted1D.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Kernel/Particle2Vertex.h"


#include "Event/TrackUnitsConverters.h"
#include "LHCbMath/MatrixUtils.h"

/** @class ParticleFlow4Jets ParticleFlow4Jets.h
 *  
 *  Algorithm for ParticleFlow. 
 *
 *  @author Victor Coco
 *  @date   2011-05-10
 */
class ParticleFlow4Jets : public GaudiTupleAlg {
public: 

  /// Standard constructor
  ParticleFlow4Jets( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ParticleFlow4Jets( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  enum BannedCaloCluster { InfMom, TrackMatch ,TrackMatchTT ,TrackMatchHCAL ,TrackMatchHCALTT , Bremsstrahlung ,
                           Photon  , CnvPhoton  , ResolvedPi0 , MergedPi0 , ChargedHadron } ;

  enum ChargedOperation { Unknown, RejectDoNotBanCluster , RejectBanCluster, KeepInPF, KeepInPFBanned } ;

protected:

private:
  
  StatusCode loadDatas() ;

  int tagTrack( const LHCb::Track* track );

  LHCb::Particle * MakeParticle( const LHCb::ProtoParticle * pp , int banType , Particle2Vertex::WTable& table);

  double kullbeckLieblerDist(const LHCb::State& c1, const LHCb::State& c2) const;

  void relate2Vertex(const LHCb::Particle* p , Particle2Vertex::WTable& table);
  
  int numberOfSaturatedCells( const LHCb::CaloCluster* cluster , const DeCalorimeter* calo);

  LHCb::Particle * MakeNeuralParticle(int clusterID , int calo , double remainingE , int numberOfSat,double originalE, double covariance);
  

  
private:

  
  std::string m_trSelType;                                       ///< The track selector type
 
  ITrackSelector * m_trSel;                                      ///< The track selector tool

  IHitExpectation* m_ttExpectation;                          ///< The TT hit expectation tool

  std::string m_clust2TrLocationECAL;   ///< Location of the Tr 2 ECAL cluster relation table

  LHCb::Calo2Track::IClusTrTable2D* m_tableTrECAL ; ///< The Tr 2 ECAL cluster relation table

  std::string m_clust2TrLocationHCAL;   ///< Location of the Tr 2 HCAL cluster relation table

  LHCb::Calo2Track::IClusTrTable2D* m_tableTrHCAL ; ///< The Tr 2 HCAL cluster relation table

  IParticle2State* m_p2s;                                     ///< The particle to state tool

  IProtoParticleFilter* m_filterGhost ;

  IRelatedPVFinder* m_pvRelator ;

  std::vector< std::string > m_particleLocations;

  std::map< std::string , std::vector< LHCb::Particles* > > m_particleContainers;

  std::map< std::string , const LHCb::CaloClusters* > m_clusterContainers;

  std::map< std::string , LHCb::ProtoParticles* > m_protoParticles;

  LHCb::RecVertex::Range m_vertices;
  
  std::string m_verticesLocation ;

  std::string m_pf2verticesLocation ;
  std::string m_pfbanned2verticesLocation ;

  std::string m_PFOutputLocation ;

  std::string  m_PFBannedOutputLocation;

  std::string m_PFHiddenNeutralOutputLocation;

  std::string m_protoPF ;

  std::string m_calohypoPF ;

  double m_Chi2ECALCut;

  double m_Chi2HCAL0Cut;

  double m_Chi2HCAL1Cut;

  double m_Chi2HCAL2Cut;

  double m_Chi2HCAL0CutEValue;

  double m_Chi2HCAL1CutEValue;

  double m_cutInfMomTRVal;

  double m_photonIDMax4ResolvedPi0 ;

  double m_photonIDMin4ResolvedPi0 ;

  double m_photonID4Photon ;

  bool m_catchBremFromElectrons ;

  bool m_useTTHits ; 

  bool m_usePIDInfo;

  bool m_useNNGhost ;

  bool m_useHCAL;

  double m_noTTChi2PerDof ;

  bool m_alsoBanClone ;

  double m_alphaECAL ;
  double m_betaHCAL ;

  std::vector< int > m_electronPPkeys ;

  std::map< int , const LHCb::Track* > m_trackKeyToBan ;

  std::string  m_banCandidatesLocation ;

  bool m_banCandidates ;

  double m_minHCALE ;

  bool  m_banFromTTrack ;
  
  double  m_Chi2HCALCutTT ;
  
  double m_Chi2ECALCutTT;

  double m_photonID4PhotonTban;

  typedef std::map< std::string , std::pair< const IProtoParticleFilter* , const LHCb::ParticleProperty * > > ProtoMap;
  ProtoMap m_protoMap;

  int m_nSigmaE;
  
  bool m_neutralRecovery;

  const DeCalorimeter* m_ecal ;
  const DeCalorimeter* m_hcal ;
  //-----------------------------------------


};
#endif // PARTICLEFLOW4JETS_H
