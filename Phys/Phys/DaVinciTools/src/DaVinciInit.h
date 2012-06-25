// $Id$
#ifndef DAVINCIINIT_H 
#define DAVINCIINIT_H 1

// from LHCbKernel
#include "Kernel/LbAppInit.h"

// Event model
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/Track.h"
#include "Event/ProtoParticle.h"

class IGenericTool;

/** @class DaVinciInit DaVinciInit.h
 *
 *  Algorithm to initialize DaVinci
 *
 *  @author Patrick Koppenburg
 *  @date   2009-03-02
 */
class DaVinciInit : public LbAppInit 
{

public: 

  /// Standard constructor
  DaVinciInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DaVinciInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  /// Release memory pools
  void releaseMemoryPools() const;

private:

  IGenericTool* m_memoryTool;     ///< Pointer to (private) memory histogram tool
  bool m_print ;                  ///< Print event and run
  unsigned long long m_increment; ///< Number of events to measure memory on
  unsigned long long m_lastMem;   ///< Last memory usage
  unsigned long long m_memPurgeLimit; ///< Memory limit to trigger a purge of the pools

};

#endif // DAVINCIINIT_H
