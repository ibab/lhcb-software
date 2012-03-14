// $Id: $
#ifndef DUMPPARTICLE_H 
#define DUMPPARTICLE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"


/** @class DumpParticle DumpParticle.h
 *  
 * Print a Particle
 *
 *  @author Patrick Koppenburg
 *  @date   2012-03-14
 */
class DumpParticle : public DVAlgorithm {
public: 
  /// Standard constructor
  DumpParticle( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DumpParticle( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // DUMPPARTICLE_H
