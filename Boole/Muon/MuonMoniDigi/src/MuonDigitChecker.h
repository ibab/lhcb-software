// $Id: MuonDigitChecker.h,v 1.4 2005-09-30 15:23:13 asarti Exp $
#ifndef MuonDigitChecker_H 
#define MuonDigitChecker_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/GaudiHistoAlg.h"

// for Muons
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonTools/IMuonTileXYZTool.h"
#include "MuonTools/IMuonGetInfoTool.h"  
#include "MuonKernel/MuonTile.h"

//From event
#include "Event/EventHeader.h"
#include "Event/MCHit.h"   
#include "Event/MCMuonHit.h"   
#include "Event/MCMuonDigit.h"   
#include "Event/MuonDigit.h"  

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

  int nhit[5][4],cnt[5][4];
  int nDhit[5][4][6],Dcnt[5][4][6];

  bool m_hitMonitor;

};
#endif // MuonDigitChecker_H
