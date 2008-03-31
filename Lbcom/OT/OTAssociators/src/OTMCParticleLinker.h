// $Id: OTMCParticleLinker.h,v 1.2 2008-03-31 16:30:52 janos Exp $
#ifndef OTASSOCIATORS_OTMCPARTICLELINKER_H 
#define OTASSOCIATORS_OTMCPARTICLELINKER_H 1

// Include files
// from STD
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from MCEvent
#include "Event/MCParticle.h"
#include "Event/MCOTTime.h"

/** @class OTMCParticleLinker OTMCParticleLinker.h
 *  
 *  Algorithm to link OTChannelIDs to OT MCParticles.
 *
 *  The MC linked particles for a given channelID(key) can be retrieved, for example, in the following way 
 *  
 *  @code
 *  #include "Event/MCParticle.h"
 *  #include "Kernel/OTChannelID.h"
 *  #include "Event/OTTime.h"
 *  #include "Linker/LinkedTo.h"
 *
 *  ...
 *
 *  LinkedTo<LHCb::MCParticle> myLink(evtSvc(), msgSvc(), LHCb::OTTimeLocation::Default);
 *  if (myLink.notFound()) Error( "Failed to find OT MCParticles linker table", StatusCode::FAILURE ); 
 *  
 *  /// Get OTTimes
 *  LHCb::OTTimes* times = get<LHCb::OTTimes>( LHCb::OTTimeLocation::Default );
 *
 *  LHCb::OTTimes::const_iterator iTime = times->begin();
 *  for ( ; iTime != times->end(); ++iTime ) {
 *    /// Get channelID(key) from OTTime
 *    unsigned int key = unsigned((*iTime)->channel());
 *    
 *    typedef std::vector<LHCb::MCParticle*> Particles
 *    /// Get particles
 *    Particles range = myLink.range(key);
 *    Particles::const_iterator iParticle = range.begin();
 *    for ( ; iParticle != range.end(); ++iParticle) {
 *      ...
 *    } 
 *    ...
 *   }
 *  @endcode
 *  
 *  For the reverse one can do
 *
 *  @code
 *  #include "Linker/LinkedFromKey.h"
 *  ...
 * 
 *  typedef LinkedFromKey<MCParticle, OTChannelID> LinkerTable;
 *  LinkerTable aLinkerTable(evtSvc(), msgSvc(), LHCb::OTTimeLocation::Default);
 *  if (aLinkerTable.notFound()) Error( "Failed to find OT MCParticle linker table", StatusCode::FAILURE );
 *
 *  for (MCParticles::const_iterator iMCParticle = mcParCont->begin(); iMCParticle != mcParCont->end(); ++iMCParticle) {
 *    std::vector<OTChannelID> channels = aLinkerTable.keyRange((*iMCParticle));
 *    for (std::vector<OTChannelID>::const_iterator iChannel = channels.begin(); 
 *          iChannel != channels.end(); ++iChannel) {
 *      ...
 *    }
 *  }
 *  @endcode
 *
 *  @author Jan Amoraal
 *  @date   2007-07-02
 */

class OTMCParticleLinker : public GaudiAlgorithm {

public: 
  /// Standard constructor
  OTMCParticleLinker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTMCParticleLinker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  /// Handy typedef
  typedef std::vector<const LHCb::MCParticle*> Particles;

  /** Particles to link for a given channelID/MCOTTime
   * Note there is a 1-to-1 correspondence between OTTime and MCOTTime, i.e. between the channelIDs. The
   * MCOTTimes are written to the RawBuffer and the decoding of the RawBuffer produces OTTimes.
   * 
   * @param aTime An MCOTTime which contains the channelID and is the same as the channelID stored in the corresponding OTTime
   * @param particles Reference to particle vector, i.e. hits to link to
   * @return StatusCode
   */   
  StatusCode particlesToLink(const LHCb::MCOTTime* aTime, Particles& particles) const;
  
  /** Output location, i.e. where to put the linker table
   * @return string
   */
  const std::string outputLocation() const;
  
  /** Input location, i.e. location of OT MChits linker table
   * @return string
   */
  const std::string inputLocation() const;
  
  std::string m_outputLocation; ///< Ouptut location of linker table
  std::string m_inputLocation;  ///< Location of MC OTDeposits linker table
};

inline const std::string OTMCParticleLinker::outputLocation() const {
  return m_outputLocation;
}

inline const std::string OTMCParticleLinker::inputLocation() const {
  return m_inputLocation;
}

#endif // OTASSOCIATORS_OTMCPARTICLELINKER_H
