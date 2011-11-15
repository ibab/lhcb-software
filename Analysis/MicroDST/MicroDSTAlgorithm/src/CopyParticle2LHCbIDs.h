// $Id: CopyParticle2LHCbIDs.h,v 1.2 2010-08-20 09:47:07 jpalac Exp $
#ifndef COPYPARTICLE2LHCBIDS_H 
#define COPYPARTICLE2LHCBIDS_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"

class ITriggerTisTos;

namespace DaVinci 
{
  namespace Map 
  {
    class Particle2LHCbIDs;
  }
}

/** @class CopyParticle2LHCbIDs CopyParticle2LHCbIDs.h
 *  
 *  MicroDSTAlgorithm to take sets of LHCb::Particles, get an std::vector
 *  containing all the LHCb::LHCbIDs that contribured to the particle, and
 *  place a Particle->std::vector<LHCb::LHCbID> map on the TES.
 *
 *  Property <b>FullDecayTree</b> extends this to all the decay products of 
 *  each input particle. The maps are places in a TES location obtained from 
 *  <OutputPrefix>/Particle2LHCbIDMap.
 *
 *  @author Juan Palacios
 *  @date   2010-08-18
 */

class CopyParticle2LHCbIDs : public MicroDSTAlgorithm 
{

public: 

  /// Standard constructor
  CopyParticle2LHCbIDs( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyParticle2LHCbIDs( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  void executeLocation(const std::string& particleLocation);

  void storeLHCbIDs(const LHCb::Particle* part) ;

private:

  std::string m_outputLocation;

  bool m_fullTree;

  ITriggerTisTos* m_iTisTos;

  DaVinci::Map::Particle2LHCbIDs* m_p2LHCbID;
  
};

#endif // COPYPARTICLE2LHCBIDS_H
