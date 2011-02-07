// $Id: $
#ifndef CALODIGITFILTERALG_H 
#define CALODIGITFILTERALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "CaloDAQ/ICaloDigitFilterTool.h"


/** @class CaloDigitFilterAlg CaloDigitFilterAlg.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2010-12-21
 */
class CaloDigitFilterAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloDigitFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloDigitFilterAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  bool mask(int& val){return (val&0x1) != 0;}
  bool offset(int& val){return (val&0x2) !=0;}

private:
  int m_ecal;
  int m_hcal;
  int m_prs;
  int m_spd;
  ICaloDigitFilterTool* m_filter;
};
#endif // CALODIGITFILTERALG_H
