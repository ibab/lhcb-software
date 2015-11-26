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
  int  val(int v1,int v2,int v3=0);
  bool check(unsigned long long  val, std::pair<double,double> range);
  bool check(int  val, std::pair<int,int> range);
  bool def(std::pair<double,double> range);
  bool def(std::pair<int,int> range);
  std::pair<int,int> m_yRange;
  std::pair<int,int> m_mRange;
  std::pair<int,int> m_dRange;
  std::pair<int,int> m_hRange;
  std::pair<int,int> m_mnRange;
  std::pair<int,int> m_sRange;
  std::pair<int,int> m_nsRange;
  std::pair<int,int> m_bRange;
  std::pair<double,double> m_eRange;
  bool m_evt;
  bool m_time;
  bool m_bx;
  bool m_loc;
  bool m_print;
};
#endif // ODINTIMEFILTER_H
