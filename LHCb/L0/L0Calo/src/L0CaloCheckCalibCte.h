// $Id: L0CaloCheckCalibCte.h,v 1.1 2009-11-24 17:42:50 robbep Exp $
#ifndef L0CALO_L0CALOCHECKCALIBCTE_H 
#define L0CALO_L0CALOCHECKCALIBCTE_H 1

// Include files
// from Calo
#include "CaloUtils/Calo2Dview.h"

class DeCalorimeter ;
class ICaloDataProvider ;
class ICaloL0DataProvider ;

/** @class L0CaloCheckCalibCte L0CaloCheckCalibCte.h
 *
 *  Monitoring algorithm to check that the L0 calorimeter
 *  constants are loaded correctly.
 *
 *  @author Patrick Robbe
 *  @date   2009-11-21
 */
class L0CaloCheckCalibCte : public Calo2Dview {
public: 
  /// Standard constructor
  L0CaloCheckCalibCte( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0CaloCheckCalibCte( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  /// Compute L0ADC from ADC 
  int l0adcFromAdc( const int adc , const LHCb::CaloCellID & id ) const ;

  /// Detector Name (Ecal or Hcal), set by job options
  std::string m_detectorName ;

  /// Name of the readout tool, set by job options
  std::string m_readoutTool ;

  /// Name of the L0 readout tool, set by job options
  std::string m_l0readoutTool ;

  /// Pointer to the Ecal or Hcal Detector Element
  DeCalorimeter * m_calo ;

  /// Tool to decode calo banks (ADCs)
  ICaloDataProvider * m_daq ;

  /// Tool to decode calo banks (L0ADCs)
  ICaloL0DataProvider * m_l0daq ;
};
#endif // L0CALO_L0CALOCHECKCALIBCTE_H
