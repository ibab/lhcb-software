// $Id: STTAEClusterTuple.h,v 1.4 2009-12-06 13:28:52 mtobin Exp $
#ifndef STTAECLUSTERTUPLE_H 
#define STTAECLUSTERTUPLE_H 1

// Include files
// from Gaudi
#include "Kernel/STTupleAlgBase.h"

namespace LHCb{
  class STCluster;
}
class TH2D;

/** @class STTAEClusterTuple STTAEClusterTuple.h
 *  
 *
 *  @author Mark Tobin
 *  @date   2009-03-10
 *
 *  Implementation of tupleing algorithm for TAE events.  
 *  Dump clusters from each spill to tuple for the different service boxes.
 *
 */
class ISTReadoutTool;
namespace ST
{
  class STTAEClusterTuple : public TupleAlgBase {
  public: 
  /// Standard constructor
    STTAEClusterTuple( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~STTAEClusterTuple( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:

  private:
    /// Book histograms

    std::vector<std::string> m_clusterLocations; ///< Input location for clusters
    unsigned int m_nSamples; ///< Number of input locations
    double m_maxSample; ///< define the max bin of samples sample
    std::vector<std::string> m_spills;///< Vector of all possible spill location for TAE events

    /// Cut on the bunch ID (distinguish Beam from cosmics)
    std::vector<unsigned int> m_bunchID;

    bool m_debug; ///< true if message service level is debug
    bool m_verbose; ///< true if message service level is verbose

    bool m_skipShortThick;///< skip the thick short ladders (IT only).
  };
} // End of ST namespace
#endif // STTAECLUSTERTUPLE_H
