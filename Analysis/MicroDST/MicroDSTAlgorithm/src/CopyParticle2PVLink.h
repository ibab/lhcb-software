// $Id: CopyParticle2PVLink.h,v 1.2 2007-10-23 14:45:06 jpalac Exp $
#ifndef COPYPARTICLE2PVLINK_H 
#define COPYPARTICLE2PVLINK_H 1

// Include files
// from Gaudi
#include "MicroDST/CopyAndStoreData.h"

// from DaVinci
#include "Kernel/Particle2Vertex.h"

/** @class CopyParticle2PVLink CopyParticle2PVLink.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-15
 */
class CopyParticle2PVLink : public CopyAndStoreData {
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
