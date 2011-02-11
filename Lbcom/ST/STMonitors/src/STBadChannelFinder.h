// $Id: STBadChannelFinder.h,v 1.2 2009/07/06 17:30:58 mtobin Exp $
#ifndef STBADCHANNELFINDER_H 
#define STBADCHANNELFINDER_H 1

// Include files
// from Gaudi

namespace LHCb{
  class STCluster;
}
#include <ostream>

namespace ST {
  class ISTNoiseCalculationTool;
  class STNoiseFlag {
  public:
    enum Status{OK, Open, LowNoise};
    /// special serializer to ASCII stream
    std::ostream& fillStream(std::ostream& s) const;
  };
  std::ostream & operator << (std::ostream & s, ST::STNoiseFlag::Status e) {
    switch (e) {
    case STNoiseFlag::OK       : return s << "OK";
    case STNoiseFlag::Open     : return s << "Open";
    case STNoiseFlag::LowNoise : return s << "LowNoise";
    default : return s << "ERROR wrong value for enum ST::STNoiseFlag::Status";
    }
  }
}

// from STKernel
#include "Kernel/STHistoAlgBase.h"

/** @class STBadChannelFinder STBadChannelFinder.h
 *  
 *
 *  @author Mark Tobin
 *  @date   2009-10-26
 *
 *  Re-implementation of TTOpenFinder from Viktor/Johan.
 */
class ISTReadoutTool;
namespace ST
{
  class STBadChannelFinder : public HistoAlgBase {
  public: 
  /// Standard constructor
    STBadChannelFinder( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~STBadChannelFinder( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:

  private:

    bool m_debug; ///< true if message service level is debug
    bool m_verbose; ///< true if message service level is verbose

    unsigned int m_maxEvents;///< Number of events to use

    ISTNoiseCalculationTool* m_noiseTool; ///< Tool to calculate noise
    std::string m_noiseToolType; ///< Tool type (default is STNoiseCalculationTool)
    std::string m_noiseToolName; ///< Tool name (default is TTNoiseCalculationTool)
    
    /// Calculate slope and intercept of line for channels in a beetle port
    void calcSlope(std::vector<double>::const_iterator noiseBegin, std::vector<double>::const_iterator noiseEnd); 
    double m_slope; ///< Slope of line fitted to channels in beetle port
    double m_intercept; ///< y-intercept of line fitted to channels in beetle port

    double m_rawNoiseOpen;///< Cut value to find open channels from raw noise 
    double m_rawNoiseLow;///<  Cut value to find channels with low noise from raw noise 

    bool m_plotBySector;///< Make 2d plots showing number of bad bonds per sector
    bool m_compareWithDB;///< Compare bad channels with DB status
    void compareWithDB();///< Compare bad channels to DB status

    void findBadChannels();

    unsigned int m_nTELL1s;///< NUmber of TELL1s = readoutTool()->nBoard();

    // Store number of bad channels per TELL1, 2nd number in pair is flag of bad channel type
    typedef std::pair<unsigned int, STNoiseFlag::Status> ChannelStatus;
    std::map<unsigned int,std::vector<ChannelStatus> > m_badChannelsPerTELL1;

    void bookHistograms();///< Book the histograms
    AIDA::IHistogram1D* m_1d_nBad;///< Number of channels failing any cuts vs TELL1
    AIDA::IHistogram1D* m_1d_nOpen;///< Number of open noise channels vs TELL1
    AIDA::IHistogram1D* m_1d_nLowNoise;///< Number of low noise channels vs TELL1
    AIDA::IHistogram2D* m_2d_nBad;///< Number of channels failing any cuts in each sector vs TELL1
    AIDA::IHistogram2D* m_2d_nOpen;///< Number of open strips in each sector vs TELL1
    AIDA::IHistogram2D* m_2d_nLowNoise;///< Number of low noise strips in each sector vs TELL1
    AIDA::IHistogram1D* m_1d_nDifferent;///< Number of channels with status different from DB
    
  };
} // End of ST namespace
#endif // STBADCHANNELFINDER_H
