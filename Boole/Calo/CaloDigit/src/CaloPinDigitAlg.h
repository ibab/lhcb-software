#ifndef CALOPINDIGITALG_H 
#define CALOPINDIGITALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "CaloDet/DeCalorimeter.h"
#include "GaudiKernel/IRndmGenSvc.h" 
/** @class CaloPinDigitAlg CaloPinDigitAlg.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-02-06
 */
class CaloPinDigitAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloPinDigitAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloPinDigitAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

  IRndmGenSvc* rndmSvc() const  { return m_rndmSvc ; } 

private:

  std::string m_detectorName;
  std::vector<double> m_signal;
  std::vector<double> m_spread;
  DeCalorimeter* m_calo;
  std::string m_data;
  std::string m_outputData;
  double m_cNoise;
  double m_iNoise;
  int m_saturateAdc;
  int m_rate;
  int m_count;
  bool m_separatePinContainer;
  
  mutable IRndmGenSvc*   m_rndmSvc;        ///< random number service 


};
#endif // CALOPINDIGITALG_H
