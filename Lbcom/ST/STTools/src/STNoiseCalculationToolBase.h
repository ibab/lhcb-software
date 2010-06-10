// $Id: STNoiseCalculationToolBase.h,v 1.3 2009-11-25 11:25:11 mtobin Exp $
#ifndef STCMNOISECALCULATIONTOOLBASE_H 
#define STCMNOISECALCULATIONTOOLBASE_H 1

// Include files
// from Gaudi
#include "Kernel/STHistoToolBase.h"
#include "Kernel/ISTNoiseCalculationTool.h"            // Interface

/** @class STNoiseCalculationToolBase STNoiseCalculationToolBase.h
 *
 *  Base class for calculation of noise tools.  Implements all data accessors
 *  to the noise and job options of the derived tools.  Data is stored internally 
 *  using the TELL1 source ID.
 *
 *  @author Mark Tobin
 *  @date   2009-10-01
 */
namespace ST { 
  class STNoiseCalculationToolBase : virtual public ST::ISTNoiseCalculationTool, public ST::HistoToolBase {
  private:

  public: 
    /// Standard constructor
    STNoiseCalculationToolBase( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);

    virtual ~STNoiseCalculationToolBase( ); ///< Destructor

    virtual StatusCode initialize(); ///< Tool initialisation

    virtual StatusCode updateNoise();///< Update the noise calculation

    /// Return an iterator corresponding to the pedestal value of the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator pedestalBegin( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the pedestal value of the last channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator pedestalEnd( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the RAW RMS noise on the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator rawNoiseBegin(const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the RAW RMS noise on the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator rawNoiseEnd(const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the RAW mean ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator rawMeanBegin( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the RAW mean ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator rawMeanEnd( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the RAW mean squared ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator rawMeanSquaredBegin( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the RAW mean squared ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator rawMeanSquaredEnd( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the number of events containing data in the first PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator rawNEventsBegin( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the number of events containing data in the last PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator rawNEventsEnd( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the CMS RMS noise on the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsNoiseBegin(const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the CMS RMS noise on the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsNoiseEnd(const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the CMS mean ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsMeanBegin( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the CMS mean ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsMeanEnd( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the CMS mean squared ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsMeanSquaredBegin( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the CMS mean squared ADC value for the first channel for a given TELL1 source ID
    virtual std::vector<double>::const_iterator cmsMeanSquaredEnd( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the number of events containing data in the first PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator cmsNEventsBegin( const unsigned int TELL1SourceID ) const = 0;

    /// Return an iterator corresponding to the number of events containing data in the last PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator cmsNEventsEnd( const unsigned int TELL1SourceID ) const = 0;

    /// Return the period of the an exponential moving average. (Set to -1 to have a cumulative average.)
    virtual int followPeriod() const {return m_followingPeriod;};

    /// Rate at which the counters for noise calculation are reset (in number of events) (Set to -1 to do no reset (default)).
    virtual int resetRate() const {return m_resetRate;};
  
    /** Number of events to be skipped. Useful when running over
        common-mode-subtracted data where the pedestals have not been calculated. **/
    virtual int skipEvents() const {return m_skipEvents;};

    /// Return an iterator corresponding to the source ID of the first TELL1 in the event containing an NZS bank
    virtual std::vector<unsigned int>::const_iterator tell1WithNZSBegin( ) const;
    
    /// Return an iterator corresponding to the source ID of the last TELL1 in the event containing an NZS bank
    virtual std::vector<unsigned int>::const_iterator tell1WithNZSEnd( ) const;

  public:

    virtual std::vector<double> rawMean(const unsigned int TELL) const = 0;//{return m_meanMap->find(TELL);} ;
    virtual std::vector<double> rawMeanSq(const unsigned int TELL) const = 0; //{return m_meanSqMap.find(TELL);} ;
    virtual std::vector<double> rawNoise(const unsigned int TELL) const = 0;//{return m_noiseMap.find(TELL);} ; 
    virtual std::vector<unsigned int> rawN(const unsigned int TELL) const = 0; //{return m_nEvents.find(TELL);} ;
    
    virtual std::vector<double> cmsMean(const unsigned int TELL) const = 0;//{return m_meanMap.find(TELL);} ;
    virtual std::vector<double> cmsMeanSq(const unsigned int TELL) const = 0;//{return m_meanSqMap.find(TELL);} ;
    virtual std::vector<double> cmsNoise(const unsigned int TELL) const = 0;//{return m_noiseMap.find(TELL);} ;
    virtual std::vector<unsigned int> cmsN(const unsigned int TELL) const = 0; //{return m_nEvents.find(TELL);} ;
    
  protected:
    /// Internal counter for noise calculation
    int                 m_evtNumber;

    /// Store the RAW values of the mean, mean squared and the noise for each strip
    typedef std::map<unsigned int, std::vector<double> > DataMap;  
    DataMap m_rawPedestalMap;        ///< Internal map for the ADC values before pedestal substraction
    DataMap m_rawMeanMap;            ///< Internal map for the pedestals
    DataMap m_rawMeanSqMap;          ///< Internal map of the pedestal^2
    DataMap m_rawNoiseMap;           ///< Internal map of the current noise

    /// Internal map of number of events per tell1 and FPGA-PP used in the calculation of the RAW noise.
    std::map<unsigned int, std::vector<unsigned int> > m_rawNEvents;

    // jobOptions:

    /// Location in the Event Data Store with the ADC values
    std::string m_dataLocation;

    /// Period of the an exponential moving average.
    /// Set to -1 to have a cumulative average.
    int m_followingPeriod;

    /// Rate at which the counters for histograms are reset (in number of events).
    /// Set to -1 to do no reset (default).
    int m_resetRate;
  
    /// Number of events to be skipped. Useful when running over
    /// common-mode-subtracted data where the pedestals have not been calculated. 
    int m_skipEvents;

    /// List of tell1s in the event which contain an NZS bank
    std::vector<unsigned int> m_tell1WithNZS;

    std::vector< unsigned int > m_limitToTell;/// List of TELL1s to look at
    bool   m_selectedTells;///< Use only selected TELL1s
    
    bool m_countRoundRobin;///< True if you want to plot number of events per PP in round robin
    void countRoundRobin(unsigned int TELL1SourceID, unsigned int PP);///< Plot histogram of number of round robin events

  private:
    virtual StatusCode calculateNoise() = 0;

    bool m_firstEvent;///< First call to noise calculation
    ulonglong  m_eventNumber;///< Current event number
    unsigned int m_runNumber;///< Current run number

    /// Keep running total of number of PPs which send the NZS banks
    AIDA::IHistogram2D* m_2d_nEventsPerPP;
    
  };
}
#endif // STCMNOISECALCULATIONTOOLBASE_H
