// $Id: STNoiseToolbase.h,v 1.3 2009-11-25 11:25:11 mtobin Exp $
#ifndef STCMNOISETOOLBASE_H 
#define STCMNOISETOOLBASE_H 1

// Include files

// from Gaudi
#include "Kernel/STHistoToolBase.h"
#include "Kernel/ISTNoiseCalculationTool.h"            // Interface

/** @class STNoiseToolBase STNoiseToolBase.h
 *
 *  Base class for noise calculation tools.  Implements all data accessors
 *  to the noise and job options of the derived tools.  Data is stored internally 
 *  using the TELL1 source ID.  The noise is calculated in each derived class using a
 *  cumulative average or an exponential moving average which can be configured as
 *  described below.   
 *
 *  The following options are configured via the base class
 *  - \b InputData: The default input data is the full NZS bank
 *  - \b FollowPeriod: This is the period of the exponential moving average. It
 *    determines the lifetime of the averages (in number of events). As long as
 *    the number of processed events is smaller than FollowPeriod the average
 *    is a cumulative average. Set this to -1 to always use a cumulative
 *    averaging.
 *  - \b ResetRate: Rate at which the counters for the noise calculation are reset
 *    (in number of events). Set to -1 to do no reset (default).
 *  - \b SkipEvents: Number of events to be skipped. Useful when running over
 *     common-mode-subtracted data where the pedestals have not been calculated.
 *  - \b PedestalsFromDB: read the pedestals from the conditions database.
 *  - \b ThresholdsFromDB: read the hit tresholds for outlier removal from the 
 *    conditions data base.
 *  - \b MaskBadChannels: set all pedestals, noise etc to 0 if channel is know to be bad
 *  - \b BadChannelsFromDB: read the masked channels from the STCOND database (otherwise 
 *    they are taken from the detector element).
 *  - \b CondPath: The pedestals and hit thresholds are not in the standard LHCBCOND 
 *    so they must be read from an additional DB slice.  The location in the database
 *    can be specified via CondPath
 *  - \b LimitToTell: the noise calculations limited to certain TELL1s.  The input here
 *    is the actual TELL1 number (TT 1-48, IT 1-42).  The conversion to the TELL1 source ID is 
 *    performed internally.
 *  - \b UseEventsPerStrip: the number of events used in the noise calculation can be stored per
 *    strip.  The default calculation assumes uses number of events per PP.
 *
 *  @author Mark Tobin
 *  @date   2009-10-01
 */
namespace ST { 
  class STNoiseToolBase : virtual public ST::ISTNoiseCalculationTool, public ST::HistoToolBase {
  private:

  public: 
    /// Standard constructor
    STNoiseToolBase( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);

    virtual ~STNoiseToolBase( ); ///< Destructor

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
    virtual std::vector<unsigned int>::const_iterator rawNEventsPPBegin( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the number of events containing data in the last PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator rawNEventsPPEnd( const unsigned int TELL1SourceID ) const;

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

    /** Return an iterator corresponding to the RMS noise after pedestal subtraction on the first
        channel for a given TELL1 source ID **/
    virtual std::vector<double>::const_iterator pedSubNoiseBegin(const unsigned int TELL1SourceID ) const;

    /** Return an iterator corresponding to the RMS noise after pedestal subtraction on the last 
        channel for a given TELL1 source ID **/
    virtual std::vector<double>::const_iterator pedSubNoiseEnd(const unsigned int TELL1SourceID ) const;

    /** Return an iterator corresponding to the mean ADC value after pedestal subtraction for the 
        first channel for a given TELL1 source ID **/
    virtual std::vector<double>::const_iterator pedSubMeanBegin( const unsigned int TELL1SourceID ) const;

    /** Return an iterator corresponding to the mean ADC value after pedestal subtraction for the last 
        channel for a given TELL1 source ID **/
    virtual std::vector<double>::const_iterator pedSubMeanEnd( const unsigned int TELL1SourceID ) const;

    /** Return an iterator corresponding to the mean squared ADC value after pedestal subtraction for the first 
        channel for a given TELL1 source ID **/
    virtual std::vector<double>::const_iterator pedSubMeanSquaredBegin( const unsigned int TELL1SourceID ) const;

    /** Return an iterator corresponding to the mean squared ADC value after pedestal subtraction for the last 
        channel for a given TELL1 source ID **/
    virtual std::vector<double>::const_iterator pedSubMeanSquaredEnd( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the number of events containing data in the first PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator pedSubNEventsPPBegin( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the number of events containing data in the last PP for a given TELL1 source ID
    virtual std::vector<unsigned int>::const_iterator pedSubNEventsPPEnd( const unsigned int TELL1SourceID ) const;

    /** Return an iterator corresponding to the number of events used in the calculation of noise (RAW, CMS, pedSub)
        after outlier removal for the first channel of a given TELL1 source ID **/
    virtual std::vector<unsigned int>::const_iterator nEventsBegin( const unsigned int TELL1SourceID ) const;

    /** Return an iterator corresponding to the number of events used in the calculation of noise (RAW, CMS, pedSub)
        after outlier removal for the last channel of a given TELL1 source ID **/
    virtual std::vector<unsigned int>::const_iterator nEventsEnd( const unsigned int TELL1SourceID ) const;

    /// Return an iterator corresponding to the status of the first channel for a given TELL1 source ID
    virtual std::vector<bool>::const_iterator stripStatusBegin( const unsigned int TELL1SourceID ) const;
    
    /// Return an iterator corresponding to the status of the last channel for a given TELL1 source ID
    virtual std::vector<bool>::const_iterator stripStatusEnd( const unsigned int TELL1SourceID ) const;

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

    /// Return the path for the conditions database
    virtual std::string conditionsPath() const { return m_condPath; };

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

    DataMap m_cmsMeanMap;            ///< Local calculation of mean of ADCs after Common Mode Suppression
    DataMap m_cmsMeanSqMap;          ///< Local calculation of mean of ADCs squared after Common Mode Suppression
    DataMap m_cmsNoiseMap;           ///< Internal map of the noise after Common Mode Suppression

    DataMap m_pedSubMeanMap;            ///< Local calculation of mean of ADCs after Pedestal Subtraction
    DataMap m_pedSubMeanSqMap;          ///< Local calculation of mean of ADCs squared after Pedestal Subtraction
    DataMap m_pedSubNoiseMap;           ///< Internal map of the noise after Pedestal Subtraction

    /// Internal map of number of events per tell1 and FPGA-PP used in the calculation of the noise.
    /// Number of events in RAW noise calculation
    std::map<unsigned int, std::vector<unsigned int> > m_rawNEventsPP;
    /// Number of events in CMS noise calculation
    std::map<unsigned int, std::vector<unsigned int> > m_cmsNEventsPP;
    /// Number of events in noise calculation after pedestal subtraction
    std::map<unsigned int, std::vector<unsigned int> > m_pedSubNEventsPP;

    /// Cache the status of each strip (1==OK, 0!=OK)
    typedef std::map<unsigned int, std::vector<bool> > StatusMap;
    StatusMap m_statusMap;

    // Use number of events per strip in noise calculations
    bool m_evtsPerStrip;
    typedef std::map<unsigned int, std::vector<unsigned int> > EventCountMap;
    EventCountMap m_nEvents;///< Number of events/strip used in raw+cms noise calculation after outlier removal

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
    
    std::vector<unsigned int> m_steps;/// List of steps to look at (useful for CCEScan)
    bool m_selectedSteps;///< Use only selected steps

    bool m_resetRunChange;///< Reset noise counters after run change

    bool m_countRoundRobin;///< True if you want to plot number of events per PP in round robin
    void countRoundRobin(unsigned int TELL1SourceID, unsigned int PP);///< Plot histogram of number of round robin events

    bool m_readPedestals;///< Read pedestals from conditions database
    bool m_readThresholds;///< Read hit thresholds from conditions database
    std::string m_condPath; ///< Set the condition path in the database
    
    bool m_maskBad;///< Turn on/off masking of bad channels
    bool m_readPedestalMask;///< Read masked strips from the conditions database slice

    /** data values for up to 8 pedestal values 
        - tellID to access map
        - vector of 8 PCN/header combinations
        - pair<pedestal, n events> */
    typedef std::map<int, std::vector< std::vector<std::pair<double, int> > > > PedestalMaps;
    PedestalMaps m_pedestalMaps;

    /// Strip thresholds for outlier rejection: first=raw, second=CMS
    typedef std::map<int, std::vector< std::pair<double, double> > > ThresholdMap;
    ThresholdMap m_thresholdMap;

  private:
    virtual StatusCode calculateNoise() = 0;

    bool m_firstEvent;///< First call to noise calculation
    ulonglong m_eventNumber;///< Current event number
    unsigned int m_runNumber;///< Current run number

    /// Keep running total of number of PPs which send the NZS banks
    AIDA::IHistogram2D* m_2d_nEventsPerPP;
    
  private:
    /// Cache the TELL1 parameters
    virtual StatusCode cacheTELL1Parameters();

    /// Read the TELL1 parameters from the conditions database for a given TELL1
    void readTELL1Parameters(const unsigned int TELL1SourceID);

    /// Reset noise counters for a given tell1 during initialise or after change in TELL1 conditions
    void resetNoiseCounters(const unsigned int TELL1SourceID);

    /// Reset all noise counters
    void resetNoiseCounters();

    bool m_debug;///< True if message service is set for DEBUG or VERBOSE
    bool m_verbose;///< True if message service is set for VERBOSE
  };
}
#endif // STCMNOISETOOLBASE_H
