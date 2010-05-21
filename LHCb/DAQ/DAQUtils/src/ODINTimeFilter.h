// $Id: ODINTimeFilter.h,v 1.1 2009/10/16 14:16:26 odescham Exp $
#ifndef ODINTIMEFILTER_H 
#define ODINTIMEFILTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
//from LHCb
#include "Event/ODIN.h"

/** @class ODINTimeFilter ODINTimeFilter.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2009-10-07
 */
class ODINTimeFilter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  ODINTimeFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ODINTimeFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  void criteriaPrintOut();
  ulonglong val(double v1,double v2,double v3=0);
  bool check(ulonglong  val, std::pair<double,double> range);
  bool def(std::pair<double,double> range);
  std::pair<double,double> m_yRange;
  std::pair<double,double> m_mRange;
  std::pair<double,double> m_dRange;
  std::pair<double,double> m_hRange;
  std::pair<double,double> m_mnRange;
  std::pair<double,double> m_sRange;
  std::pair<double,double> m_nsRange;
  std::pair<double,double> m_eRange;
  std::pair<double,double> m_bRange;
  bool m_evt;
  bool m_time;
  bool m_bx;
  bool m_loc;
  bool m_print;
};
#endif // ODINTIMEFILTER_H
