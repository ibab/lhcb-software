#ifndef STDUMPCLUSTERS_H 
#define STDUMPCLUSTERS_H 1

// Include files
// from Gaudi
#include "Kernel/STTupleAlgBase.h"

/** @class STDumpClusters STDumpClusters.h
 *  
 * Dump the STClusters into an ntuples containing:
 *
 * "run"                   : run number
 * "event"                 : event number
 * "nClus"                 : Total number of clusters
 * "tell1[m_Clusters]",    : TELL1 board which produced cluster
 * "tellChan[m_Clusters]", : TELL1 channel of the highest strip (0 to 3071)
 * "channelID[m_Clusters]",: STChannelID of the highest strip
 * "strip[m_Clusters]",    : strip number of the highest strip (1 to 512)
 * "station[m_Clusters]",  : station number from the STChannelID for the highest strip
 * "layer[m_Clusters]",    : layer number from the STChannelID for the highest strip
 * "detRegion[m_Clusters]",: detRegion from the STChannelID for the highest strip
 * "sector[m_Clusters]",   : sector number from the STChannelID for the highest strip
 * "fracStrip[m_Clusters]",: interstrip fraction 
 * "size[m_Clusters]",     : cluster size
 * "charge[m_Clusters]",   : total charge of the cluster
 * "strip1[m_Clusters]",   : strip number 1
 * "strip2[m_Clusters]",   : strip number 2
 * "strip3[m_Clusters]",   : strip number 3
 * "strip4[m_Clusters]",   : strip number 4
 * "adc1[m_Clusters]",     : ADC value of strip1
 * "adc2[m_Clusters]",     : ADC value of strip2
 * "adc3[m_Clusters]",     : ADC value of strip3
 * "adc4[m_Clusters]",     : ADC value of strip4
 *
 * The size of the array can be changed via the job option MaximumNumberClusters
 *
 *  @author Mark Tobin
 *  @date   2010-03-15
 */

namespace LHCb{
  class STCluster;
}

namespace ST 
{
  class STDumpClusters : public TupleAlgBase {
  public: 
    /// Standard constructor
    STDumpClusters( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~STDumpClusters( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution

  protected:
    
  private:
    std::string m_clusterLocation; ///< Input location for clusters
    unsigned int m_nClusters; ///< Defines the maximum number of clusters expected

    bool m_dumpMap;///< Build mapping to TELL1 and sectors
    std::vector<bool> m_sectorTELL1s;///<<< Use to build a map of cluster sector to TELL1
    void dumpClusterMapping(const LHCb::STCluster* cluster);
  };
}
#endif // STDUMPCLUSTERS_H
