// $Id: L0Monitor.h,v 1.4 2004-09-20 14:27:36 cattanem Exp $
#ifndef L0MONITOR_H 
#define L0MONITOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

// Forward declarations
class IPhysSelTool;
class IAssociator;

/** @class  L0Monitor L0Monitor.h
 *  Produces Tuples to store L0 trigger decision
 *
 *  @author Olivier Callot
 *  @date   29/06/2001
 */
class L0Monitor : public Algorithm {
public:
  /// Standard constructor
  L0Monitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0Monitor( ); ///< Destructor

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  double m_totEvt;
  double m_goodL0;
  double m_puVeto;

  double m_trigElec;
  double m_trigPhot;
  double m_trigHadr;
  double m_trigPi0L;
  double m_trigPi0G;
  double m_trigPi0 ;
  double m_trigMuon;
  double m_trigSumMu;

  double m_exclElec;
  double m_exclPhot;
  double m_exclHadr;
  double m_exclPi0L;
  double m_exclPi0G;
  double m_exclPi0 ;
  double m_exclMuon;
  double m_exclSumMu;

  double m_minPeak2Veto;

  bool         m_fillNtuple;
  std::string  m_tupleName;
  
  NTuple::Item<float>      m_decis;
  NTuple::Item<float>      m_elec;
  NTuple::Item<float>      m_phot;
  NTuple::Item<float>      m_pi0l;
  NTuple::Item<float>      m_pi0g;
  NTuple::Item<float>      m_hadr;
  NTuple::Item<float>      m_had2;
  NTuple::Item<float>      m_sume;
  NTuple::Item<float>      m_spdm;
  NTuple::Item<float>      m_pile;
  NTuple::Item<float>      m_emu1;
  NTuple::Item<float>      m_emu2;
  NTuple::Item<float>      m_emu3;
  NTuple::Item<float>      m_sumu;

  NTuple::Item<float>      m_tElec;
  NTuple::Item<float>      m_tPhot;
  NTuple::Item<float>      m_tPi0;
  NTuple::Item<float>      m_tHadr;
  NTuple::Item<float>      m_tMuon;
};
#endif // L0MONITOR_H
