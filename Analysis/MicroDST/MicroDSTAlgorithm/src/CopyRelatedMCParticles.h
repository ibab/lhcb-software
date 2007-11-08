// $Id: CopyRelatedMCParticles.h,v 1.2 2007-11-08 16:48:25 jpalac Exp $
#ifndef COPYRELATEDMCPARTICLES_H 
#define COPYRELATEDMCPARTICLES_H 1

// Include files
// from Gaudi
#include "MicroDST/CopyAndStoreData.h"

class MCParticle;
class MCVertex;

/** @class CopyRelatedMCParticles CopyRelatedMCParticles.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-02
 */
class CopyRelatedMCParticles : public CopyAndStoreData {
public: 
  /// Standard constructor
  CopyRelatedMCParticles( const std::string& name, 
                          ISvcLocator* pSvcLocator );

  virtual ~CopyRelatedMCParticles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  LHCb::MCParticle* storeMCParticle(const LHCb::MCParticle* particle);

  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  const LHCb::MCVertex* storeMCVertex(const LHCb::MCVertex* vertex);
};
#endif // COPYRELATEDMCPARTICLES_H
