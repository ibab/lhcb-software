// $Id: DecodeVeloRawBuffer.h,v 1.11 2010-04-13 15:21:12 dhcroft Exp $

#ifndef DECODEVELORAWBUFFER_H 
#define DECODEVELORAWBUFFER_H 1

#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"

#include "SiDAQ/SiRawBufferWord.h"

namespace LHCb {
  class RawBank;
}

class DeVelo;

/** @class DecodeVeloRawBuffer DecodeVeloRawBuffer.h
 *  Decode the Velo data from the 1MHZ Raw buffer.
 *  Completely re-written for the new 1Mhz raw buffer.
 *
 *  @author David Jones
 *  @author Kurt Rinnert
 *  @author Chris Parkes
 *  @date   2004-02-02
 */
class DecodeVeloRawBuffer : public GaudiAlgorithm {
public:

  /// Standard constructor
  DecodeVeloRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DecodeVeloRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalisation

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

private:

  // configuration

  bool m_decodeToVeloLiteClusters;
  bool m_decodeToVeloClusters;

  bool m_dumpVeloClusters;

  unsigned int m_forcedBankVersion; ///< user forced bank version

  std::string m_rawEventLocation;
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

};
#endif // DECODEVELORAWBUFFER_H
