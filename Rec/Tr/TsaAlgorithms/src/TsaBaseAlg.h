// $Id: TsaBaseAlg.h,v 1.3 2006-09-18 13:42:59 mneedham Exp $
#ifndef _TSABASEALG_H_
#define _TSABASEALG_H_

/** @class TsaBaseAlg
 *
 *  Class for Initialization
 *
 *  @author M.Needham
 *  @date   07/03/2005
 */

//#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>

//#include "TsaKernel/stopwatch.h"

class TsaBaseAlg: public GaudiAlgorithm {

public:
  
  // Constructors and destructor
  TsaBaseAlg(const std::string& name, 
                   ISvcLocator* pSvcLocator); 
  virtual ~TsaBaseAlg();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode finalize();  

 protected:
 
  //void startTimer();
  // void stopTimer();
  //  Tsa::stopWatch& timer() const;

 private:

  // bool m_time;
  // mutable Tsa::stopWatch m_stopWatch; 

  // unsigned int m_nEvent;

};

/*
inline Tsa::stopWatch& TsaBaseAlg::timer() const{
  return m_stopWatch;
}

inline void TsaBaseAlg::startTimer() {
  if (m_nEvent != 0) timer().start();
}

inline void TsaBaseAlg::stopTimer(){
  if (m_nEvent != 0) timer().stop();
  ++m_nEvent;
}
*/

#endif // _TSABASEALG_H_











