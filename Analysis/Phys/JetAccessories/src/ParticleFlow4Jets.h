// $Id: $
#ifndef PARTICLEFLOW4JETS_H 
#define PARTICLEFLOW4JETS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// interfaces
#include "TrackInterfaces/ITrackSelector.h"
#include "TrackInterfaces/IHitExpectation.h"
// Event
#include "Event/Track.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
#include "Kernel/IProtoParticleFilter.h"
#include "GaudiKernel/VectorMap.h"

#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

/** @class ParticleFlow4Jets ParticleFlow4Jets.h
 *  
 *  Algorithm for ParticleFlow. 
 *
 *  @author Victor Coco
 *  @date   2011-05-10
 */
class ParticleFlow4Jets : public GaudiAlgorithm {
public: 

  /// Standard constructor
  ParticleFlow4Jets( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ParticleFlow4Jets( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  enum BannedCaloCluster { TrackMatch , Bremsstrahlung , Photon  , ResolvedPi0 , MergedPi0 } ;

protected:

private:
  StatusCode getContainers() ;
  bool selectTrack( const LHCb::Track* track );
  LHCb::Particle * MakeParticle( const LHCb::ProtoParticle * pp );
  
 
  
private:
  
  std::string m_trSelType;  ///< The track selector type
 
  ITrackSelector * m_trSel; ///< The track selector tool

  IHitExpectation* m_ttExpectation; ///< The TT hit expectation tool
  
  std::vector< std::string > m_particleLocations;
  std::map< std::string , LHCb::Particles* > m_particleContainers;

  std::string m_PFOutputLocation ;

  std::string m_clust2TrLocation;
  
  double m_Chi2CaloCut;
  
  double m_cutInfMomTRVal;

  double m_photonID4ResolvedPi0 ;

  double m_photonID4Photon ;

  bool m_catchBremFromElectrons ;

  double m_noTTChi2PerDof ;

  typedef std::pair< const LHCb::ParticleProperty *, const IProtoParticleFilter* > ProtoPair;
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
#endif // PARTICLEFLOW4JETS_H
