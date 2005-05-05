// $Id: MuonHitChecker.h,v 1.1.1.1 2005-05-05 09:42:12 asarti Exp $
#ifndef MuonHitChecker_H 
#define MuonHitChecker_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/GaudiHistoAlg.h"

// for Muons
#include "MuonTools/MuonChannel.h"
#include "MuonTools/IMuonGetInfoTool.h"  
#include "MuonKernel/MuonTile.h"
#include "Event/MCMuonHit.h"   

/** @class MuonHitChecker MuonHitChecker.h
 *  
 *
 *  @author A Sarti
 *  @date   2005-05-20
 */
class MuonHitChecker : public GaudiTupleAlg {
public:
  /// Standard constructor
  MuonHitChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonHitChecker( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  static std::string spill[5];
  static std::string numreg[4];   
  static std::string numsta[5];

  int m_stationNumber;
  std::vector<int> m_numberOfGaps;
  int m_regionNumber;
  int m_partition;

private:

  IHistogram1D * m_pointer1Dradial[20];
  IHistogram1D * m_pointer1Dtime[20];

  virtual StatusCode crNtuples();
  int nhit[5][4],cnt[5][4];

  bool m_detailedMonitor;
  /// N-tuple items to be written away (hit)
  long  m_run;
  long  m_evt;
  long  m_nhits;
  std::vector<float> m_sta;
  std::vector<float> m_reg;
  std::vector<float> m_con;
  std::vector<float> m_x;
  std::vector<float> m_y;
  std::vector<float> m_z;
  std::vector<float> m_time;
  std::vector<float>  m_id;
  std::vector<float> m_px;
  std::vector<float> m_py;
  std::vector<float> m_pz;
  std::vector<float> m_E;
  std::vector<float> m_xv;
  std::vector<float> m_yv;
  std::vector<float> m_zv;
  std::vector<float> m_tv;
  std::vector<float>  m_mom;
  std::vector<float> m_ple;
  std::vector<float> m_hen;
  std::vector<float> m_dix;
  std::vector<float> m_dxz;
  std::vector<float> m_dyz;

};
#endif // MuonHitChecker_H
