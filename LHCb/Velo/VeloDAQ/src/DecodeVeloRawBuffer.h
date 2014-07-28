#ifndef DECODEVELORAWBUFFER_H 
#define DECODEVELORAWBUFFER_H 1

#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"
#include "DAQKernel/DecoderAlgBase.h"

#include "SiDAQ/SiRawBufferWord.h"

namespace LHCb {
  class RawBank;
}

class DeVelo;
class IIncidentSvc;

/** @class DecodeVeloRawBuffer DecodeVeloRawBuffer.h
 *  Decode the Velo data from the 1MHZ Raw buffer.
 *  Completely re-written for the new 1Mhz raw buffer.
 *
 *  @author David Jones
 *  @author Kurt Rinnert
 *  @author Chris Parkes
 *  @date   2004-02-02
 */
class DecodeVeloRawBuffer : public Decoder::AlgBase {
public:

  /// Standard constructor
  DecodeVeloRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DecodeVeloRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

  enum AlgStatusType{ 
    OK = 0,
    BadTELL1IDMapping = 1,
    UnsupportedBufferVersion = 2,
    CorruptVeloBuffer = 3,
    TooManyClusters = 4,
    HeaderErrorBit = 5,
    Other = 99
  };
    

private:

  /** Decode raw buffer to lite clusters  
   * This decodes the raw buffer to VeloLiteClusters and
   * adds a VeloLiteCluster::FastContainer to the TES.
   *
   * @see VeloLiteCluster
   */
  StatusCode decodeToVeloLiteClusters(const std::vector<LHCb::RawBank*>& banks);

  /** Decode raw buffer to clusters  
   * This decodes the raw buffer to VeloClusters and
   * adds themn to the TES.
   *
   * @see VeloCluster
   */
  StatusCode decodeToVeloClusters(const std::vector<LHCb::RawBank*>& banks);

  /** Write VeloClusters to stdout   
   * 
   *  @see VeloCluster
   */
  void dumpVeloClusters(const LHCb::VeloClusters* clusters) const;

  /** Create empty banks
   *
   * Creates empty cluster and lite cluster banks on the TES.
   * This called as a failsafe option in case the raw event 
   * is missing.
   * */
  void createEmptyBanks(); 

  /** Add DecodeVeloRawBuffer to list of failed algorithms
   *  if procAbort = true set the ProcStatus to "aborted" to show 
   *  this event should be removed from physics streams
   */
  void failEvent(const std::string &ErrorText,
                 const std::string &ProcText,
                 AlgStatusType status,
                 bool procAborted);  

  /** Replace the full clusters for a specific sensor with faked 
   *  clusters from the lite container
   */
  StatusCode replaceFullFromLite(LHCb::VeloClusters *clusters,
                                 unsigned int nSensor,
                                 const std::vector<LHCb::RawBank*>& banks);

  /// Add a fake lite cluster to the full cluster container
  void makeFakeCluster(LHCb::VeloLiteCluster const &liteCluster,
                       LHCb::VeloClusters* fakeClusters);

private:

  // configuration

  bool m_decodeToVeloLiteClusters;
  bool m_decodeToVeloClusters;

  bool m_dumpVeloClusters;

  unsigned int m_forcedBankVersion; ///< user forced bank version
  
  std::string m_veloLiteClusterLocation;
  std::string m_veloClusterLocation;

  /// do we assume chip channels instead of strips in the raw buffer?
  /// (useful for some testbeam TELL1 cinfigurations, defaults to false)
  bool m_assumeChipChannelsInRawBuffer;

  /// helpers
  const DeVelo* m_velo;

  /// maximum permissible number of VELO clusters, 
  /// more than this will force an IncidentType::AbortEvent
  unsigned int m_maxVeloClusters;

  /// m_errorCount is the number of errors per fault to print
  /// default = 0
  /// 10 is added to it if the message level is debug
  unsigned int m_errorCount;

  /// if true, clusters will be decoded even in the presence of errors
  bool m_ignoreErrors;

  /// if true hide the errors from multiple cluster using the same strip
  bool m_hideWarnings;

  /// Check when decoding lite clusters that the bank length is correct
  bool m_doLengthCheck;

  IIncidentSvc* m_incidentSvc;  ///< Pointer to the incident service.

  /// default raw event locations: not set in options to allow comparison
  std::vector<std::string> m_defaultRawEventLocations;

};
#endif // DECODEVELORAWBUFFER_H
