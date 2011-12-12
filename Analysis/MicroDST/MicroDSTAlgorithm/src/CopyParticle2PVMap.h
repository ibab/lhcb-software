// $Id: CopyParticle2PVMap.h,v 1.1 2010-08-26 17:06:45 jpalac Exp $
#ifndef COPYPARTICLE2PVMAP_H 
#define COPYPARTICLE2PVMAP_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"

class ICloneVertexBase;

/** @class CopyParticle2PVMap CopyParticle2PVMap.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2010-08-26
 */
class CopyParticle2PVMap : public MicroDSTAlgorithm 
{
public: 

  /// Standard constructor
  CopyParticle2PVMap( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyParticle2PVMap( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  void executeLocation(const std::string& particleLocation);

private:

  ICloneVertexBase* m_toCloner;

};
#endif // COPYPARTICLE2PVMAP_H
