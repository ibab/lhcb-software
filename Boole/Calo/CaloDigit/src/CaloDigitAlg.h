// $Id: CaloDigitAlg.h,v 1.10 2010-01-15 16:06:31 robbep Exp $ 
#ifndef   CALODIGIT_CALODIGITALG_H
#define   CALODIGIT_CALODIGITALG_H 1
// ============================================================================
// from Gaudi 
#include "GaudiKernel/IRndmGenSvc.h" 
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/RndmGenerators.h"

// CaloDet
#include "CaloDet/DeCalorimeter.h"

// FPE guard to protect Rndm::Poisson
#include "Kernel/FPEGuard.h"

/** @class CaloDigitAlg CaloDigitAlg.h   
 *
 *  a (sub)Algorithm responsible 
 *  for digitisation of MC-information 
 *  
 *  @author: Olivier Callot
 *   @date:   5 June 2000
 */

class CaloDigitAlg : public GaudiHistoAlg {  

public:
  CaloDigitAlg( const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~CaloDigitAlg();  ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize();
protected:
  IRndmGenSvc* rndmSvc() const  { return m_rndmSvc ; } 

private:
  std::string m_detectorName;      ///< Detector element location
  std::string m_inputData;         ///< Input container
  std::string m_outputData;        ///< Output container
  std::string m_inputPrevData;     ///< Input data (prev.BX)
  std::string m_triggerName;       ///< Name of the trigger bank

  DeCalorimeter*         m_calo;           ///< Detector element pointer
  mutable IRndmGenSvc*   m_rndmSvc;        ///< random number service 
 
  double      m_coherentNoise     ; ///< magnitude of coherent noise 
  double      m_incoherentNoise   ; ///< magnitude of incoherent noise
  double      m_gainError         ; ///< error in PMT gain
  double      m_fracPrev          ; ///< fraction of previous BX subtracted
  double      m_pedestalShift     ; ///< pedestal shift due to noise
  double      m_deadCellFraction  ; ///< Fraction of dead cells
  double      m_triggerEtScale;    ///< Scale for trigger ADC
  double      m_triggerThreshold  ;  ///< Zero suppress for trigger
  double      m_zSupThreshold;     ///< Zero suppression for data, PreShower only
  bool        m_triggerIsBit;      ///< Trigger data is a bit bank
  double      m_mip ; // MIP deposit (Prs/Spd)
  double      m_sat ; // fraction of full dynamics to consider (taking ped substraction into account)
  std::vector<double>      m_phe; // number of photoelectron per MIP (for each area)
  int         m_numberOfCells     ; ///< Number of cells of this detector. 
  int         m_saturatedAdc      ; ///< Value to set in case ADC is saturated.
  int         m_maxAdcValue       ; ///< Maximum ADC content, after ped. substr
  double      m_activeToTotal     ; ///< Ratio activeE to total energy in ADC.
  bool        m_zSup;

  enum { GAUS_INTEGRAL = 0, GAUS_MEAN = 1, GAUS_SIGMA = 2 };
  bool m_monitorNoise; ///< create noise monitoring histogram for each section
  bool m_useAdvancedNoise; ///< use advanced noise spectra generation
  double m_noiseIntegral[3]; ///< integrals of noise spectra
  std::map< int, std::vector< std::vector<double> > > m_advancedNoise; /// advanced noise spectra for different areas
  AIDA::IHistogram1D* m_noiseHist[3]; ///< monitoring histograms from advanced noise

  ///protected poisson distribution against FPE
  inline double safe_poisson( double num ) {
    // turn off the inexact FPE
    FPE::Guard underFPE(FPE::Guard::mask("AllExcept"), true);
    Rndm::Numbers nPe( this->rndmSvc(), Rndm::Poisson( num ) );
    return nPe();
  }

  int caloTriggerFromAdc( const int adc , const LHCb::CaloCellID id ) const ;
};

#endif //    CALODIGIT_CALODIGITALG_H
