// $Id: STOnlineNoiseCalculationTool.h,v 1.2 2009-11-09 17:51:38 mtobin Exp $
#ifndef STONLINENOISECALCULATIONTOOL_H 
#define STONLINENOISECALCULATIONTOOL_H 1

// Include files
// from Gaudi
#include "STNoiseToolBase.h"
#include "Kernel/ISTNoiseCalculationTool.h"            // Interface

/** @class STOnlineNoiseCalculationTool STOnlineNoiseCalculationTool.h
 *
 *  Calculation of the raw and CMS noise optimised for round robin NZS data 
 *  processed using the TELL1 emulator.  The tool calculates the raw and cms noise
 *  using two input locations.
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
    std::string m_pedSubLocation;///< Input location of pedestal subtracted ADC values
    bool m_removeOutliers;///< Remove outliers from noise calculation

  };
}
#endif // STONLINENOISECALCULATIONTOOL_H
