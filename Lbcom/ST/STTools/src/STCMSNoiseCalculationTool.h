// $Id: STCMSNoiseCalculationTool.h,v 1.3 2009-11-25 11:25:11 mtobin Exp $
#ifndef STCMSNOISECALCULATIONTOOL_H 
#define STCMSNOISECALCULATIONTOOL_H 1

// Include files
#include "Event/STTELL1Data.h"

// from Gaudi
#include "STNoiseToolBase.h" // Base class for 
#include "Kernel/ISTNoiseCalculationTool.h"            // Interface

/** @class STCMSNoiseCalculationTool STCMSNoiseCalculationTool.h
 *
 *  This tool can be used to calculate raw noise from the input data.  It then has various 
 *  different methods which can be used to perform pedestal and common mode subtraction on
 *  the raw ADC values.  The algorithm should *not* be used with round robin events.  It is 
 *  useful to compare CMS algorithms when the full NZS data is available for each TELL1.
 *
 *  The following options can be configured for this algorithm:
 *
 *  @author J. van Tilburg, N. Chiapolini
 *  @date   10/02/2009
 *  
 *  Reimplementation of noise calculation in cms branch of STNZSMonitor as a Tool
 *
 *  @author Mark Tobin
 *  @date   2009-10-01
 */
namespace ST { 
  class STCMSNoiseCalculationTool : virtual public ST::ISTNoiseCalculationTool, public ST::STNoiseToolBase {

  public: 
    /// Standard constructor
    STCMSNoiseCalculationTool( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);

    virtual ~STCMSNoiseCalculationTool( ); ///< Destructor

    virtual StatusCode initialize(); ///< Tool initialisation

  private:
    virtual StatusCode calculateNoise();///< calculation of noise

  protected:

  private:

    /// Calculate the different PCN/header configuration for each Beetle port
    void calcPCNConfigs( const LHCb::STTELL1Datas* data );
    std::vector<unsigned long> m_portHeader;/// PCN/Header configuration for each port in this event

    /// Calculation of pedestals
    void sumPedestals(const LHCb::STTELL1Datas* data );
    /// Something to do with pedestals
    void divPedestals();

    unsigned int m_pedestalBuildup;///< Number of events used to build pedestals
    
    bool m_skipCMS;///< Skip calculation of CMS noise

  public:
    virtual std::vector<double> rawMean(const unsigned int TELL) const;
    virtual std::vector<double> rawMeanSq(const unsigned int TELL) const;
    virtual std::vector<double> rawNoise(const unsigned int TELL) const;
    virtual std::vector<unsigned int> rawN(const unsigned int TELL) const;
    
    virtual std::vector<double> cmsMean(const unsigned int TELL) const;
    virtual std::vector<double> cmsMeanSq(const unsigned int TELL) const;
    virtual std::vector<double> cmsNoise(const unsigned int TELL) const;
    virtual std::vector<unsigned int> cmsN(const unsigned int TELL) const;
    
  private:
    /// Calculate the pedestal subtracted ADC values for one beetle
    void subtractPedestals(const std::vector<signed int>& BeetleADCs, 
                            std::vector<double>::const_iterator rawMeanIt,
                            std::vector<double>::const_iterator rawNoiseIt,
                            const signed int beetle,
                            std::vector<std::pair <double, bool> >& pedSubADC,
                            std::vector<double>& BeetleMeans);
    
    /// Remove PCN/Header corrected pedestals from raw adcs
    void subtractPCNPedestals( const std::vector<signed int>& RawADCs, 
                                const std::vector< std::vector<std::pair<double, int> > >* pedestals, 
                                const signed int beetle,
                                std::vector<signed int>& PedSubADCs);

    /// Calculate the CM suppressed values for the strips given
    void cmsSimple(std::vector<std::pair <double, bool> >& BeetleTmpADCs, std::vector<double>& BeetleMeans);

    /// Explain what this does? TO DO
    void tell1CMSIteration( std::vector<std::pair <double, bool> >& BeetleTmpADCs, 
                            std::vector<double>& BeetleMeans,
                            int iPort );

    /// Calculate the CM suppressed values for the strips given
    void cmsST(std::vector<std::pair <double, bool> >& BeetleTmpADCs, std::vector<double>& BeetleMeans);

    /// Explain what this does? TO DO
    void stIteration( std::vector<std::pair <double, bool> >& BeetleTmpADCs, 
                            std::vector<double>& BeetleMeans,
                            int iPort );

    /// Different routines to calculate the CMS noise in integer form (a la TELL1)
    void cmsSTInt( std::vector<std::pair <double, bool> >& BeetleTmpADCs );
    void stTell1Code( std::vector<std::pair <double, bool> >& BeetleTmpADCs,  int iPort );
    void saturate_for_8bits(int *data, int line_numb);


    /// Noise calculation in the style of the VELO
    void cmsVelo( std::vector<std::pair <double, bool> >& BeetleTmpADCs,
                  std::vector<double>& BeetleMeans);
    void veloHitDetection1( std::vector<std::pair <double, bool> >& BeetleTmpADCs,
                            std::vector<double>& BeetleMeans, int iPort );
    void veloHitDetection2( std::vector<std::pair <double, bool> >& BeetleTmpADCs, int iPort );
    double calcThreshold( std::vector<std::pair <double, bool> >::iterator& itBeetleTmpADCs, 
                          double nSigma);

    /// Noise calculation using the Achim method
    void cmsAchim ( std::vector<std::pair <double, bool> >& BeetleTmpADCs );
    void achimIteration( std::vector<std::pair <double, bool> >& BeetleTmpADCs, int iPort );

    bool m_dump;///< Dump ADC values at various stages of the processing
    unsigned int m_printCMSSteps;///< Number of CMS steps to print
    bool m_printADCs;///< Print the ADCs at various stages of the processing

    /// Methods to dump ADCs at various stages of the processing.
    void dumpBeetleADCs(const std::vector<std::pair <double, bool> >& BeetleTmpADCs, std::string s); 
    void dumpBeetleADCs(const std::vector<signed int>& BeetleADCs, std::string s);
    void dumpPortADCs(const std::vector<std::pair <double, bool> >& BeetleTmpADCs, int port, std::string s);
    void dumpPortADCs(const std::vector<double>& adc, int port, 
                      std::string s);
  


    /** Choose the CMS algorithm to use
        Supported algorithms are:
        Simple
        ST
        STInt
        VELO
        Achim
    **/
    std::string m_cmsAlg; 

    int  m_useAlgo;///< Internal representation of algorithm being used
    void whichCMSAlg();
    int  m_useInts; ///< Use integers for the calculation of pedestals/cms noise

    bool m_printAlgError;///< 

    /// Threshold value for raw outlier rejection. value  in standard deviations.
    int m_skipOutliers;///< Number of events to skip before starting outlier removal
    bool m_rawOutliers; // Remove outliers in RAW noise calculation
    bool m_rawOutliersCMS; // Remove outliers in CMS noise calculation
    int  m_rawThreshold;///< Default value is 4
    
    double m_cmsThreshold;///< Threshold for removal of hits from CMS noise
    bool m_fixedThreshold;///< Use fixed threshold in outlier removal for CMS noise
    bool m_zeroNeighbours;///< 
    
    bool m_cmsHitDetection;///< Use the CMS Hit detection

    /// Set adc value to zero when 
    void zeroHitsUseRawNoise( std::vector<std::pair <double, bool> >::iterator& itBeetleTmpADCs );
    void zeroHitsNormal( std::vector<std::pair <double, bool> >::iterator& itBeetleTmpADCs, double threshold );
    void zeroHits( std::vector<std::pair <double, bool> >::iterator& itBeetleTmpADCs, double threshold );

    void plotPedestals();
  };
}
#endif // STCMSNOISECALCULATIONTOOL_H
