// $Id: CopyParticle2PVLink.h,v 1.1 2007-10-22 16:29:50 jpalac Exp $
#ifndef COPYPARTICLE2PVLINK_H 
#define COPYPARTICLE2PVLINK_H 1

// Include files
// from Gaudi
#include "MicroDST/CopyAndStoreData.h"


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

  std::string m_inputParticleLocation;
  std::string m_inputPVLocation;
  
};
#endif // COPYPARTICLE2PVLINK_H
