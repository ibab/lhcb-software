// $Id: V0MakerAlg.h,v 1.2 2004-01-14 19:00:06 gcorti Exp $
#ifndef V0MAKERALG_H 
#define V0MAKERALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

class ParticleID;
class IParticleStuffer;
class IParticlePropertySvc;

/** @class V0MakerAlg V0MakerAlg.h
 *  Algorithm to read V0 objects and transform them into Particles,
 *  Vertices and ProtoParticles related to a Kshort or Lambda.
 * 
 *  @author Gloria Corti
 *  @date   2002-12-02
 */
class V0MakerAlg : public Algorithm {
public:
  /// Standard constructor
  V0MakerAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~V0MakerAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  void fillProtoP( TrStoredTrack* aTrack, ProtoParticle* aProto );
  StatusCode fillParticle( ProtoParticle* aProto, int protoID,      
                           Particle* aParticle );

private:
  std::string m_inputPath;           ///< TES location of input V0
  // TES location of all output objects, /Particles, /Vertices, /Protos are
  // added to this path 
  std::string m_outputPath;
  
  std::string m_v0Name;              ///< Particle name of V0 (K0S or Lambda)
  ParticleID m_v0ID;                 ///< Particle ID of V0
  IParticleStuffer* m_pStuffer;      ///< Reference to ParticleStuffer
  IParticlePropertySvc* m_ppSvc;     ///< Reference to ParticlePropertyService
  
};
#endif // V0MAKERALG_H
