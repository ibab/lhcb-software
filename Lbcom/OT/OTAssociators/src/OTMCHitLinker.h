// $Id: OTMCHitLinker.h,v 1.1 2007-07-02 16:15:29 janos Exp $
#ifndef OTASSOCIATORS_OTMCHITLINKER_H 
#define OTASSOCIATORS_OTMCHITLINKER_H 1

// Include files
// from STD
#include <string>
#include <vector> 

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from MCEvent
#include "Event/MCHit.h"
#include "Event/MCOTTime.h"

/** @class OTMCHitLinker OTMCHitLinker.h
 *     
 *  Algorithm to link OTChannelIDs to OT MCHits. One may also link 
 *  spill over hits by setting the option SpillOver to true. 
 *  
 *  @code
 *  OTMCHitLinker.SpillOver = true; // default == false
 *  @endcode
 *
 *  The MC linked hits for a given channelID(key) can be retrieved, for example, in the following way 
 *  
 *  @code
 *  #include "Event/MCHit.h"
 *  #include "Kernel/OTChannelID.h"
 *  #include "Event/OTTime.h"
 *  #include "Linker/LinkedTo.h"
 *
 *  ...
 *
 *  LinkedTo<LHCb::MCHit> myLink(evtSvc(), msgSvc(), LHCb::OTTimeLocation::Default+"2MCHits");
 *  if (myLink.notFound()) Error( "Failed to find OT MCHits linker table", StatusCode::FAILURE ); 
 *  
 *  /// Get OTTimes
 *  LHCb::OTTimes* times = get<LHCb::OTTimes>( LHCb::OTTimeLocation::Default );
 *
 *  LHCb::OTTimes::const_iterator iTime = times->begin();
 *  for ( ; iTime != times->end(); ++iTime ) {
 *    /// Get channelID(key) from OTTime
 *    unsigned int key = unsigned((*iTime)->channel());
 *    
 *    typedef std::vector<LHCb::MCHit*> Hits
 *    /// Get hits
 *    Hits range = myLink.range(key);
 *    Hits::const_iterator iHit = range.begin();
 *    for ( ; iHit != range.end(); ++iHit) {
 *      ...
 *    } 
 *    ...
 *   }
 *  @endcode
 *
 *  @author Jan Amoraal
 *  @date   2007-07-02
 */

class OTMCHitLinker : public GaudiAlgorithm {

public: 
  /// Standard constructor
  OTMCHitLinker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTMCHitLinker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  /// Handy typedef
  typedef std::vector<const LHCb::MCHit*> Hits;

  /** Hits to link for a given channelID/MCOTTime
   * Note there is a 1-to-1 correspondence between OTTime and MCOTTime, i.e. between the channelIDs. The
   * MCOTTimes are written to the RawBuffer and the decoding of the RawBuffer produces OTTimes.
   * 
   * @param aTime An MCOTTime which contains the channelID and is the same as the channelID stored in the corresponding OTTime
   * @param mcHits To check if hit belongs to current container
   * @param hits Reference to hit vector, i.e. hits to link to
   * @return StatusCode
   */   
  StatusCode hitsToLink(const LHCb::MCOTTime* aTime, const LHCb::MCHits* mcHits, Hits& hits) const;
  
  /** Output location, i.e. where to put the linker table
   * @return string
   */
  const std::string outputLocation() const;
  
  /** Input location, i.e. location of MC OTDeposits linker table
   * @return string
   */
  const std::string inputLocation() const;
  
  std::string m_outputLocation; ///< Ouptut location of linker table
  std::string m_inputLocation;  ///< Location of MC OTDeposits linker table
  bool m_spillOver;             ///< Link spill over hits?
};

inline const std::string OTMCHitLinker::outputLocation() const {
  return m_outputLocation;
}

inline const std::string OTMCHitLinker::inputLocation() const {
  return m_inputLocation;
}

#endif // OTASSOCIATORS_OTMCHITLINKER_H
