// $Id: $
#ifndef DUMPPARTICLE_H 
#define DUMPPARTICLE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"

/** @class DumpParticle DumpParticle.h
 *  
 * Print a Particle
 *
 *  @author Patrick Koppenburg
 *  @date   2012-03-14
 */
class DumpParticle : public DaVinciAlgorithm
{

public: 

  /// Standard constructor
  DumpParticle( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DumpParticle( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  StatusCode dump(const LHCb::Particle*);

};
#endif // DUMPPARTICLE_H
