// $Id: MuonDigitChecker.h,v 1.6 2005-10-04 17:02:04 asarti Exp $
#ifndef MuonDigitChecker_H 
#define MuonDigitChecker_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// for Muons
#include "MuonTools/IMuonTileXYZTool.h"

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

  int m_regionNumber;
  int m_partition;

private:
  IMuonTileXYZTool* m_MuonTileXYZ ;

  int nhit[5][4][4],cnt[5][4][4];
  int nDhit[5][4][6],Dcnt[5][4][6];

  bool m_hitMonitor;

};
#endif // MuonDigitChecker_H
