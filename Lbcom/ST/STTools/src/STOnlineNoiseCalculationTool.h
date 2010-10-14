// $Id: STOnlineNoiseCalculationTool.h,v 1.2 2009-11-09 17:51:38 mtobin Exp $
#ifndef STONLINENOISECALCULATIONTOOL_H 
#define STONLINENOISECALCULATIONTOOL_H 1

// Include files
// from Gaudi
#include "STNoiseToolBase.h"
#include "Kernel/ISTNoiseCalculationTool.h"            // Interface

/** @class STOnlineNoiseCalculationTool STOnlineNoiseCalculationTool.h
 *
 *  The noise per strip of the TELL1s is calculated for all TELL1s.  
 *  Implements abstract methods of STNoiseToolBase class.
 *  There are options for calculating the noise:
 *  - \b FollowPeriod: This is the period of the exponential moving average. It
 *    determines the lifetime of the averages (in number of events). As long as
 *    the number of processed events is smaller than FollowPeriod the average
 *    is a cumulative average. Set this to -1 to always use a cumulative
 *    averaging.
 *  - \b ResetRate: Rate at which the counters for the noise calculation are reset
 *    (in number of events). Set to -1 to do no reset (default).
 *  - \b SkipEvents: Number of events to be skipped. Useful when running over
 *     common-mode-subtracted data where the pedestals have not been calculated.
 *
 *  The noise values are stored in a map which is accessed via the source ID of the TELL1.
 *
 *  @author J. van Tilburg, N. Chiapolini
 *  @date   10/02/2009
 *  
 *  Reimplementation of noise calculation in STNZSMonitor as a Tool
 *
 *  @author Mark Tobin
 *  @date   2009-10-01
 */
namespace ST { 
  class STOnlineNoiseCalculationTool : virtual public ST::ISTNoiseCalculationTool, public ST::STNoiseToolBase {
  private:

    friend class ToolFactory<ST::STOnlineNoiseCalculationTool>;

  public: 
    /// Standard constructor
    STOnlineNoiseCalculationTool( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);

    virtual ~STOnlineNoiseCalculationTool( ); ///< Destructor

    virtual StatusCode initialize(); ///< Tool initialisation

  private:
    virtual StatusCode calculateNoise(); ///< Noise calculation

  public:
    virtual std::vector<double> rawMean(const unsigned int TELL) const;
    virtual std::vector<double> rawMeanSq(const unsigned int TELL) const;
    virtual std::vector<double> rawNoise(const unsigned int TELL) const;
    virtual std::vector<unsigned int> rawN(const unsigned int TELL) const;
    
    virtual std::vector<double> cmsMean(const unsigned int TELL) const;
    virtual std::vector<double> cmsMeanSq(const unsigned int TELL) const;
    virtual std::vector<double> cmsNoise(const unsigned int TELL) const;
    virtual std::vector<unsigned int> cmsN(const unsigned int TELL) const;

  protected:

  private:
    // Job options

    std::string m_LCMSLocation;///< Input location of LCMS ADC values
    bool m_removeOutliers;///< Remove outliers from noise calculation

  };
}
#endif // STONLINENOISECALCULATIONTOOL_H
