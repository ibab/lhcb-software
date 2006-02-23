// $Id: DecodeVeloRawBuffer.h,v 1.5 2006-02-23 18:56:35 krinnert Exp $

#ifndef DECODEVELORAWBUFFER_H 
#define DECODEVELORAWBUFFER_H 1

#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"

#include "SiDAQ/SiRawBufferWord.h"

namespace LHCb {
  class RawBank;
}

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
  void decodeToVeloLiteClusters(const std::vector<LHCb::RawBank*>& banks) const;

  /** Decode raw buffer to clusters  
   * This decodes the raw buffer to VeloClusters and
   * adds themn to the TES.
   *
   * @see VeloCluster
   */
  void decodeToVeloClusters(const std::vector<LHCb::RawBank*>& banks) const;

private:

  // configuration

  bool m_decodeToVeloLiteClusters;
  bool m_decodeToVeloClusters;

  std::string m_rawEventLocation;
  std::string m_veloLiteClusterLocation;
  std::string m_veloClusterLocation;

};
#endif // DECODEVELORAWBUFFER_H
