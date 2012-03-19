#ifndef HLT2PRESELDV_H 
#define HLT2PRESELDV_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
//#include "Kernel/DVAlgorithm.h"
#include "LoKi/Algo.h"
// from Gaudi
#include "GaudiKernel/VectorMap.h"

#include "DetDesc/IDetectorElement.h"


#include "Event/TrackUnitsConverters.h"
#include "LHCbMath/MatrixUtils.h"

#include "VeloDet/DeVelo.h"
#include "Kernel/IMatterVeto.h"   
#include "Kernel/IJetMaker.h"   

/** @class Hlt2PreSelDV Hlt2PreSelDV.h
 *  @brief Turn a given container of RecVertex if they fulfill some criteria
 *  @author Neal Gauvin
 *  @date   12 janvier 2010
 */

class Hlt2PreSelDV : public LoKi::Algo {
public: 
  /// Standard constructor
  Hlt2PreSelDV( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~Hlt2PreSelDV( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  ///Get all RecVertex from RecVerticesLocation
  void GetRecVertices( LHCb::RecVertex::ConstVector & );
  ///Get the upstream PV
  const LHCb::RecVertex * GetUpstreamPV();
  ///Turn a RecVertex into a Particle
  bool RecVertex2Particle( const LHCb::RecVertex*, 
			   const LHCb::Particle::ConstVector &, 
			     double , LHCb::ProtoParticles& );
  /// Create a map between the Particles and the Velo Tracks

  void CreateMap( const LHCb::Particle::ConstVector & );
  /// Creates a pion with 400 MeV pt from track slopes.
  const LHCb::Particle* DefaultParticle( const LHCb::Track * p );
  /// Has a RecVertex a backward track ?
  bool HasBackwardTracks( const LHCb::RecVertex* );
  /// Has a RecVertex a backward and a forward track ?
  bool HasBackAndForwardTracks( const LHCb::RecVertex* );
  

  double kullbeckLieblerDist(const LHCb::State& c1, const LHCb::State& c2) const;
  //Geometric tools
  double GetSumPt( const LHCb::Particle * );
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
  const IMatterVeto* m_materialVeto;
  bool m_computeMV ;    
  bool   m_KeepLowestZ;       ///< keep the RV with the lowest Z (particle gun)
  bool   m_UseMap;            ///< Use a map to store Particle Track relation
  bool   m_GatherParticles;   ///< Use a map to store Particle Track relation
  bool   m_MapCalled;         ///< has the map been already called in event ?
  /// Where RecVertices are stored on the TES
  std::vector<std::string> m_RVLocation ;
  std::string m_veloProtoPartLocation;  ///< Location in TES of VeloProtoparticles
  GaudiUtils::VectorMap<int, const LHCb::Particle *> m_map;
  GaudiUtils::VectorMap<int, int > m_mapLongTrack;
  double m_piMass;  ///< the pion mass
  double m_pt;      ///< default pt for default pions
  bool m_useVeloTrack;
  DeVelo* m_velo;
  double m_fractE;
  double m_fractTrUpVertex;
  double m_chi2Long;
  double m_chi2Down;

  ///To sort the reconstructed vertices according to the z position
  class CondRVz{
  public:
    bool operator() ( const LHCb::RecVertex* first, 
		      const LHCb::RecVertex* second) const {
      return first->position().z() < second->position().z();
    }
  };

  ///To sort PVs with ascending z position 
  struct sortPVz {
    bool operator() ( const LHCb::RecVertex* first, 
                      const LHCb::RecVertex* second ) { 
      return first->position().z() < second->position().z();
    }
  } SortPVz;
  
};

#endif // RECVERTICES2PARTICLES_H
