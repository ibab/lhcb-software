// $Id: MuonDigitChecker.h,v 1.2 2005-06-10 11:05:23 asarti Exp $
#ifndef MuonDigitChecker_H 
#define MuonDigitChecker_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/GaudiHistoAlg.h"

// for Muons
#include "MuonTools/IMuonTileXYZTool.h"
#include "MuonTools/MuonChannel.h"
#include "MuonTools/IMuonGetInfoTool.h"  
#include "MuonKernel/MuonTile.h"
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
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  StatusCode locateMuonTileFromXYZ(double x,double y,double z,int&
	numberTileOutput, MuonTileID phChTileID[2],bool debug=false);
/*
*
*/
protected:
  static std::string spill[5];
  static std::string numreg[4];   
  static std::string numsta[5];
  static std::string TESPathOfHitsContainer[4];
  int m_stationNumber;
  std::vector<int> m_numberOfGaps;
  int m_regionNumber;
  int m_partition;

private:
  IMuonTileXYZTool* m_MuonTileXYZ ;

  IHistogram1D * m_pointer1Dradial[20];
  IHistogram1D * m_pointer1Dtime[20];

  virtual StatusCode crNtuples();
  int nhit[5][4],cnt[5][4];
  int nDhit[5][4][6],Dcnt[5][4][6];

  bool m_hitMonitor;
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

  /// N-tuple items to be written away (Digit)
  long  m_digit_run;
  long  m_digit_evt;
  long  n_digits;
  std::vector<float> m_digit_s;
  std::vector<float> m_digit_r;
  std::vector<float> m_digit_x;
  std::vector<float> m_digit_y;
  std::vector<float> m_digit_z;
  std::vector<float> m_digit_dx;
  std::vector<float> m_digit_dy;
  std::vector<float> m_digit_dz;
  std::vector<float> m_digit_time;
  std::vector<float> m_digit_origin;
  std::vector<float> m_digit_bx;
  std::vector<float> m_digit_firing;
  std::vector<float> m_digit_multi;

};
#endif // MuonDigitChecker_H
