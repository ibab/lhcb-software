// $Id: STNoiseCalculationTool.h,v 1.2 2009-11-09 17:51:38 mtobin Exp $
#ifndef STNOISECALCULATIONTOOL_H 
#define STNOISECALCULATIONTOOL_H 1

// Include files
// from Gaudi
#include "STNoiseToolBase.h"
#include "Kernel/ISTNoiseCalculationTool.h"            // Interface

/** @class STNoiseCalculationTool STNoiseCalculationTool.h
 *
 *  The noise per strip of the TELL1s is calculated for all TELL1s.  
 *  Implements abstract methods of STNoiseToolBase class.  The noise
 *  is calculated from the ADCs given in the InputData location.
 *  In this tool, the CMS noise returned is identical to the raw noise.
 *  There is no outlier rejection performed yet so this tool should only 
 *  be used to look at the noise in low occupancy events.
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
  class STNoiseCalculationTool : virtual public ST::ISTNoiseCalculationTool, public ST::STNoiseToolBase {

  public: 
    /// Standard constructor
    STNoiseCalculationTool( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);

    virtual ~STNoiseCalculationTool( ); ///< Destructor

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

  public:
    /// Return an iterator corresponding to the CMS RMS noise on the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsNoiseBegin(const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the CMS RMS noise on the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsNoiseEnd(const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the CMS mean ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsMeanBegin( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the CMS mean ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsMeanEnd( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the CMS mean squared ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsMeanSquaredBegin( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the CMS mean squared ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsMeanSquaredEnd( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the number of events containing data in the first PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator cmsNEventsPPBegin( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the number of events containing data in the last PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator cmsNEventsPPEnd( const unsigned int TELL1SourceID ) const;

  protected:

  private:

  };
}
#endif // STNOISECALCULATIONTOOL_H
