// $Id: ISTNoiseCalculationTool.h,v 1.2 2009-11-09 17:49:46 mtobin Exp $
#ifndef KERNEL_ISTNOISECALCULATIONTOOL_H 
#define KERNEL_ISTNOISECALCULATIONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
namespace ST {
  static const InterfaceID IID_ISTNoiseCalculationTool ( "ISTNoiseCalculationTool", 1, 0 );

  /** @class ISTNoiseCalculationTool ISTNoiseCalculationTool.h Kernel/ISTNoiseCalculationTool.h
   *  
   * Abstract interface for noise calculation tool.
   *
   *  @author Mark Tobin
   *  @date   2009-10-01
   */
  class ISTNoiseCalculationTool : public virtual IAlgTool {
  public: 

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_ISTNoiseCalculationTool; }

    /// Calculate the noise for the TELL1 data
    virtual StatusCode updateNoise() = 0;

    /// Return an iterator corresponding to the pedestal value of the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator pedestalBegin( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the pedestal value of the last channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator pedestalEnd( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the RAW RMS noise on the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator rawNoiseBegin(const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the RAW RMS noise on the last channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator rawNoiseEnd(const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the RAW mean ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator rawMeanBegin( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the RAW mean ADC value for the last channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator rawMeanEnd( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the RAW mean squared ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator rawMeanSquaredBegin( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the RAW mean squared ADC value for the last channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator rawMeanSquaredEnd( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the number of events containing data in the first PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator rawNEventsPPBegin( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the number of events containing data in the last PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator rawNEventsPPEnd( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the CMS RMS noise on the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsNoiseBegin(const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the CMS RMS noise on the last channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsNoiseEnd(const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the CMS mean ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsMeanBegin( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the CMS mean ADC value for the last channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsMeanEnd( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the CMS mean squared ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsMeanSquaredBegin( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the CMS mean squared ADC value for the last channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsMeanSquaredEnd( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the number of events containing data in the first PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator cmsNEventsPPBegin( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the number of events containing data in the last PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator cmsNEventsPPEnd( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the status of the first channel for a given TELL1 source ID
    virtual std::vector<bool>::const_iterator stripStatusBegin( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the status of the last channel for a given TELL1 source ID
    virtual std::vector<bool>::const_iterator stripStatusEnd( const unsigned int TELL1SourceID ) const = 0;

    /** Return an iterator corresponding to the number of events used in the calculation of RAW+CMS noise after outlier removal
        for the first channel of a given TELL1 source ID **/
    virtual std::vector<unsigned int>::const_iterator nEventsBegin( const unsigned int TELL1SourceID ) const = 0;

    /** Return an iterator corresponding to the number of events used in the calculation of RAW+CMS noise after outlier removal
        for the last channel of a given TELL1 source ID **/
    virtual std::vector<unsigned int>::const_iterator nEventsEnd( const unsigned int TELL1SourceID ) const = 0;

    /// Return the period of the an exponential moving average. (Set to -1 to have a cumulative average.)
    virtual int followPeriod() const = 0;

    /// Rate at which the counters for noise calculation are reset (in number of events) (Set to -1 to do no reset (default)).
    virtual int resetRate() const = 0;
  
    /** Number of events to be skipped. Useful when running over
        common-mode-subtracted data where the pedestals have not been calculated. **/
    virtual int skipEvents() const = 0;

    /// Return an iterator corresponding to the source ID of the first TELL1 in the event containing an NZS bank
    virtual std::vector<unsigned int>::const_iterator tell1WithNZSBegin( ) const = 0;
    
    /// Return an iterator corresponding to the source ID of the last TELL1 in the event containing an NZS bank
    virtual std::vector<unsigned int>::const_iterator tell1WithNZSEnd( ) const = 0;

  public:
    virtual std::vector<double> rawMean(const unsigned int TELL) const = 0;// {return m_rawMean[TELL];} ;
    virtual std::vector<double> rawMeanSq(const unsigned int TELL) const = 0;// {return m_rawMeanSq[TELL];} ;
    virtual std::vector<double> rawNoise(const unsigned int TELL) const = 0;//{return m_rawNoise[TELL];} ;
    virtual std::vector<unsigned int> rawN(const unsigned int TELL) const = 0;// {return m_rawN[TELL];} ;
    
    virtual std::vector<double> cmsMean(const unsigned int TELL) const = 0;//{return m_meanMap[TELL];} ;
    virtual std::vector<double> cmsMeanSq(const unsigned int TELL) const = 0;//{return m_meanSqMap[TELL];} ;
    virtual std::vector<double> cmsNoise(const unsigned int TELL) const = 0;//{return m_noiseMap[TELL];} ;
    virtual std::vector<unsigned int> cmsN(const unsigned int TELL) const = 0;//{return m_nEvents[TELL];} ;
    
  protected:

  private:
    /// Calculate the noise for the TELL1 data
    virtual StatusCode calculateNoise() = 0;
  };
}
#endif // KERNEL_ISTNOISECALCULATIONTOOL_H
