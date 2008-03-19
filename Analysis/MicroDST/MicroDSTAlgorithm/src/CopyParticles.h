// $Id: CopyParticles.h,v 1.7 2008-03-19 12:34:42 jpalac Exp $
#ifndef COPYPARTICLES_H 
#define COPYPARTICLES_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTAlgorithm.h>
#include <MicroDST/ICloneParticle.h>
#include <Event/Particle.h>
#include <Event/Vertex.h>

/** @class CopyParticles CopyParticles.h
 *  
 *
 * 
 *  @author Juan PALACIOS
 *  @date   2007-10-16
 */
class CopyParticles : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyParticles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyParticles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  ICloneParticle* m_particleCloner;

  std::string m_particleClonerName;

};
#endif // COPYPRIMARYVERTICES_H
