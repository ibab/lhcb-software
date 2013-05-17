// $Id: MuonDigitChecker.h,v 1.7 2006-02-07 17:17:32 asarti Exp $
#ifndef MuonDigitChecker_H 
#define MuonDigitChecker_H 1

// Include files

// from Gaudi
#include "MuonDet/MuonBasicGeometry.h"   
#include "GaudiAlg/GaudiTupleAlg.h"

/** @class MuonDigitChecker MuonDigitChecker.h
 *  
 *
 *  @author A Sarti
 *  @date   2005-05-20
 */
class MuonDigitChecker : public GaudiTupleAlg {
public:
  /// Standard constructor
  MuonDigitChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonDigitChecker( ); ///< Destructor
  virtual StatusCode initialize();    /// initialization
  virtual StatusCode execute   ();    /// execution
  virtual StatusCode finalize  ();    /// finalization

protected:

private:
  int nhit[5][4][4],cnt[5][4][4];
  int nDhit[5][4][6],Dcnt[5][4][6];

  bool m_hitMonitor;
  MuonBasicGeometry* m_base;
};
#endif // MuonDigitChecker_H
