// $Id: MuBgTest.h,v 1.2 2003-10-01 14:20:34 asatta Exp $
#ifndef MUBGTEST_H 
#define MUBGTEST_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"
#include "MuonTools/IMuonGetInfoTool.h"
#include <cmath> 

/** @class MuBgTest MuBgTest.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2003-03-10
 */
class MuBgTest : public Algorithm {
public:
  /// Standard constructor
  MuBgTest( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuBgTest( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  StatusCode fillHisto(int station,int region, int ispill,
                       SmartDataPtr<MCMuonHits> hitPointer,int* track,
                       int type);
  
    protected:

private:
  int m_stationNumber;
  int m_regionNumber;
  int m_partition;  
  int m_numberOfEventsNeed;
  std::vector<std::vector <float> > containerOfFirstGapPosition;
  static std::string spill[5];
  static std::string numreg[4];
  static std::string numsta[5];
  static std::string TESPathOfHitsContainer[4];
  IMuonGetInfoTool* m_pGetInfo ;

  IHistogram1D * m_pointer1Dradial[80];
  IHistogram2D * m_pointer2Dphivsradial[80];
  IHistogram2D * m_pointer2Dphilocvsradial[80];
  IHistogram2D * m_pointer2Dthetalocvsradial[80];
  IHistogram2D * m_pointer2Dtimevsradial[80];
  IHistogram2D * m_pointer2Dtimelogvsradial[80];
  IHistogram1D * m_pointer1Dgap[80];
  
  IHistogram1D * m_sicbpointer1Dradial[80];
  IHistogram2D * m_sicbpointer2Dphivsradial[80];
  IHistogram2D * m_sicbpointer2Dtimevsradial[80];
  IHistogram2D * m_sicbpointer2Dtimelogvsradial[80];

};
#endif // MUBGTEST_H
