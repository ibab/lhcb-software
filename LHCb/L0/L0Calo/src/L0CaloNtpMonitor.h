// $Id: L0CaloNtpMonitor.h,v 1.2 2008-04-13 16:53:32 robbep Exp $
#ifndef L0CALONTPMONITOR_H
#define L0CALONTPNOMITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

class DeCalorimeter ;

/** @class L0CaloNtpMonitor L0CaloNtpMonitor.h
 *
 *  @author Marie-Helene Schune
 *  @date 2008-02-22
 */
 class L0CaloNtpMonitor : public GaudiTupleAlg {
 public:
  /// Standard constructor
  L0CaloNtpMonitor( const std::string &name , ISvcLocator * pSvcLocator ) ;
  
  virtual ~L0CaloNtpMonitor( ) ; ///< Destructor
  
  virtual StatusCode initialize() ; ///< Algorithm initialization
  virtual StatusCode execute   () ; ///< Algorithm execution
  
protected:

private: 
  std::string              m_inputDataSuffix ;

  DeCalorimeter * m_ecal            ; ///< Pointer to Ecal detector element
  DeCalorimeter * m_hcal            ; ///< Pointer to Hcal detector element

};
#endif  // L0CALONTPMONITOR_H
