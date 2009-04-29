// $Id: HltMuonRefine.h,v 1.7 2009-04-29 14:18:11 asatta Exp $
#ifndef HLTMUON_HLTMUONREFINE_H 
#define HLTMUON_HLTMUONREFINE_H 1

#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include <math.h>

// Include files
#include "Event/L0MuonCandidate.h"
#include "MuonDet/IMuonFastPosTool.h"

/** @class HltMuonRefine HltMuonRefine.h HltMuon/HltMuonRefine.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-11-24
 */
class HltMuonRefine : public HltAlgorithm {
public: 
  /// Standard constructor
  HltMuonRefine( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltMuonRefine( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
private:

  bool acceptMatch(double dist,double momentum);
  double calcDist(double slX,double slY,double X,double Y,
                  double Z,double XMuon,double YMuon,double ZMuon) {
    double distx=(slX*(ZMuon-Z)+X-XMuon);
    double disty=(slY*(ZMuon-Z)+Y-YMuon);
    return sqrt(distx*distx+disty*disty/4);
  };
  bool acceptMatchL0(double slX,double slY,double X,double Y,double Z,
                     double XMuon,double YMuon,double ZMuon,int region);
  

private:

  LHCb::Tracks* m_TESInputTracks;
  std::string m_TESInputTracksName;
  
  IMuonFastPosTool      *m_iPosTool;
  LHCb::L0MuonCandidates* m_inputL0Muon;
  
  //  std::string m_inputLongTracksName;
  // std::string m_inputMuonTracksName;
  //std::string m_outputTracksName;

  //  TrgL0MuonProvider*  m_l0MuonProvider; ///<Pointer to the muons container tool
  //TrgL0MuonContainer* m_inputL0Muons;   ///< Pointer to the L0 muons



  double m_par1;
  double m_par2;
  double m_par4;
  std::vector<double> m_L0X;
  std::vector<double> m_L0Y;
  
  bool               m_onTES; ///< Should create States ?

  // Counters
  int m_countEvents;
  int m_countRemoved;

  Hlt::SelectionContainer2<LHCb::Track,LHCb::Track>  m_selections;


};
#endif // HLTMUON_HLTMUONREFINE_H
