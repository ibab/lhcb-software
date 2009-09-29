// $Id: VeloSamplingMonitor.h,v 1.7 2009-09-29 18:27:36 krinnert Exp $
#ifndef VELORECMONITORS_VELOSAMPLINGMONITOR_H 
#define VELORECMONITORS_VELOSAMPLINGMONITOR_H 1

// Include files
// -------------
#include "VeloDet/DeVelo.h"

// from VeloEvent
#include "VeloEvent/VeloTELL1Data.h"

// from DigiEvent
#include "Event/VeloCluster.h"

#include "TH1D.h"
#include "TH2D.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// local
#include "VeloMonitorBase.h"

/** @class VeloSamplingMonitor VeloSamplingMonitor.h
 *  
 *
 *  @author Eduardo Rodrigues
 *  @date   2008-08-20
 */

namespace Velo
{
  
  class VeloSamplingMonitor : public VeloMonitorBase {

  public: 
    /// Standard constructor
    VeloSamplingMonitor( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloSamplingMonitor( );    ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
  private:
    
    // Retrieve the VeloClusters
    LHCb::VeloClusters* veloClusters( std::string samplingLocation );
    
    // Retrieve the VeloTell1Data
    LHCb::VeloTELL1Datas* veloTell1Data( std::string samplingLocation );

    // Monitor the VeloClusters
    void monitorClusters( std::string samplingLocation,
                          LHCb::VeloClusters* clusters );

    // Monitor the VeloTell1Data
    void monitorTell1Data( std::string samplingLocation,
                           LHCb::VeloTELL1Datas* tell1Datas );

    // Monitor difference in MPV of ADC between TAEs
    void monitorTAEDiff(); 

    // Data members
    const DeVelo* m_velo;

    AIDA::IHistogram2D* m_hClusADCSampR;   
    AIDA::IHistogram2D* m_hClusADCSampRC;   
    AIDA::IHistogram2D* m_hClusADCSampRA;   
    AIDA::IHistogram2D* m_hClusADCSampPhi;  
    AIDA::IHistogram2D* m_hClusADCSampPhiC; 
    AIDA::IHistogram2D* m_hClusADCSampPhiA; 
    AIDA::IHistogram2D* m_hClusADCSampPU;   
    AIDA::IHistogram2D* m_hClusADCSampPUC;  
    AIDA::IHistogram2D* m_hClusADCSampPUA;  
    AIDA::IHistogram2D* m_hClusADCSampTop;  
    AIDA::IHistogram2D* m_hClusADCSampTopC; 
    AIDA::IHistogram2D* m_hClusADCSampTopA; 
    AIDA::IHistogram2D* m_hClusADCSampBot;  
    AIDA::IHistogram2D* m_hClusADCSampBotC; 
    AIDA::IHistogram2D* m_hClusADCSampBotA; 
    AIDA::IHistogram2D* m_hChanADCSamp; 
    TH2D* m_histClusADCSampAll;
    TH1D* m_histTaeADCDiffNext;
    TH1D* m_histTaeADCDiffPrev;
    TH1D* m_histTaeADCDiffDiff;

  
    // Job options
    std::vector<std::string> m_samplingLocations;
    bool m_useNZS;

  };
  
}

#endif // VELORECMONITORS_VELOSAMPLINGMONITOR_H
