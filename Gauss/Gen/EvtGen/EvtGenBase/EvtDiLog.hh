// $Id: EvtDiLog.hh,v 1.1 2007-01-24 11:21:45 robbep Exp $
#ifndef EVTGENBASE_EVTDILOG_HH 
#define EVTGENBASE_EVTDILOG_HH 1

// Include files
#include <cmath>

/** @class EvtDiLog EvtDiLog.hh EvtGenBase/EvtDiLog.hh
 *  Dilogarithm function (replaces CERNLIB DDILOG)
 *
 *  @author Patrick Robbe
 *  @date   2007-01-23
 */
class EvtDiLog {
public:
  static double DiLog( double x ) ;

protected:

private:
  static const double Z1 = 1. ;
  static const double HF = Z1 / 2. ;
  static const double PI = 3.14159265358979324 ;
  static const double PI3 = PI * PI / 3. ;
  static const double PI6 = PI * PI / 6. ;
  static const double PI12 = PI * PI / 12. ;
  static const double C[ 20 ] ;
};
#endif // EVTGENBASE_EVTDILOG_HH
