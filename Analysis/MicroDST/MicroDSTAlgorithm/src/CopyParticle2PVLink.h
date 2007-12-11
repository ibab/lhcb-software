// $Id: CopyParticle2PVLink.h,v 1.3 2007-12-11 17:37:12 jpalac Exp $
#ifndef COPYPARTICLE2PVLINK_H 
#define COPYPARTICLE2PVLINK_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"

// from DaVinci
#include "Kernel/Particle2Vertex.h"

/** @class CopyParticle2PVLink CopyParticle2PVLink.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-15
 */
class CopyParticle2PVLink : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyParticle2PVLink( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyParticle2PVLink( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  Particle2Vertex::Table* tableOfClones(const Particle2Vertex::Table* table);

  const std::string particleLocation(const std::string& tableLocation);
  
};
#endif // COPYPARTICLE2PVLINK_H
