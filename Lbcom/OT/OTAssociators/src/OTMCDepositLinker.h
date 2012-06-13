#ifndef OTASSOCIATORS_OTMCDEPOSITLINKER_H 
#define OTASSOCIATORS_OTMCDEPOSITLINKER_H 1

// Include files
// from STD
#include <string>
#include <vector>
#include <utility>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from MCEvent
#include "Event/MCOTDeposit.h"
#include "Event/MCOTTime.h"

/** @class OTMCDepositLinker OTMCDepositLinker.h
 *   
 *  Algorithm to link OTChannelIDs to OT MCDeposits from the current spill and from spill over. 
 *  Deposits that are killed by the (Analogue/Digital) dead time may also be linked by setting 
 *  a delta time window within to accept a deposit with the option AcceptTime
 *  
 *  @code
 *  OTMCDepositLinker.AcceptTime = 9.0*ns; // == default
 *  @endcode
 *
 *  The MC linked deposits for a given channelID(key) can be retrieved, for example, in the following way 
 *  
 *  @code
 *  #include "Event/MCOTDeposit.h"
 *  #include "Kernel/OTChannelID.h"
 *  #include "Event/OTTime.h"
 *  #include "Linker/LinkedTo.h"
 *
 *  ...
 *
 *  LinkedTo<LHCb::MCOTDeposit> myLink(evtSvc(), msgSvc(), LHCb::OTTimeLocation::Default + "2MCDeposits");
 *  if (myLink.notFound()) Error( "Failed to find OT MCDeposits linker table", StatusCode::FAILURE ); 
 *  
 *  /// Get OTTimes
 *  LHCb::OTTimes* times = get<LHCb::OTTimes>( LHCb::OTTimeLocation::Default );
 *
 *  LHCb::OTTimes::const_iterator iTime = times->begin();
 *  for ( ; iTime != times->end(); ++iTime ) {
 *    /// Get channelID(key) from OTTime
 *    unsigned int key = unsigned((*iTime)->channel());
 *    
 *    typedef std::vector<LHCb::MCOTDeposit*> Deposits
 *    /// Get deposits
 *    Deposits range = myLink.range(key);
 *    Deposits::const_iterator iDeposit = range.begin();
 *    for ( ; iDeposit != range.end(); ++iDeposit) {
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
 *  typedef LinkedFromKey<MCOTDeposit, OTChannelID> LinkerTable;
 *  LinkerTable aLinkerTable(evtSvc(), msgSvc(), LHCb::OTTimeLocation::Default+"2MCDeposits");
 *  if (aLinkerTable.notFound()) Error( "Failed to find OT MCDeposits linker table", StatusCode::FAILURE );
 *
 *  for (MCOTDeposits::const_iterator iMCDeposit = mcDepCont->begin(); iMCDeposit != mcDepCont->end(); ++iMCDeposit) {
 *    std::vector<OTChannelID> channels = aLinkerTable.keyRange((*iMCDeposit));
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

class OTMCDepositLinker : public GaudiAlgorithm {

public:
 
  /// Standard constructor
  OTMCDepositLinker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTMCDepositLinker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

  /// Handy typedef
  typedef std::pair<const LHCb::MCOTDeposit*, double> Deposit;
  typedef std::vector<Deposit> Deposits;


  /** Deposits to link for a given channelID/MCOTTime
   * Note there is a 1-to-1 correspondence between OTTime and MCOTTime, i.e. between the channelIDs. The
   * MCOTTimes are written to the RawBuffer and the decoding of the RawBuffer produces OTTimes.
   * 
   * @param aTime An MCOTTime which contains the channelID and is the same as the channelID stored in the corresponding OTTime
   * @param deposits Reference to deposit vector, i.e. Deposits to link to.
   * @return StatusCode
   */   
  StatusCode depositsToLink(const LHCb::MCOTTime& aTime, Deposits& deposits) const;
  
  /** Output location, i.e. where to put the linker table
   * @return string
   */
  const std::string outputLocation() const;
  
  std::string m_outputLocation; ///< Ouptut location of linker table
  double m_acceptTime;          ///< Delta time window within to accept deposits 

};

inline const std::string OTMCDepositLinker::outputLocation() const {
  return m_outputLocation;
}

#endif //  OTASSOCIATORS_OTMCDEPOSITLINKER_H
