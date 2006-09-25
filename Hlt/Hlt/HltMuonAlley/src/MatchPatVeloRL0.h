// $Id: MatchPatVeloRL0.h,v 1.1.1.1 2006-09-25 16:29:11 asatta Exp $
#ifndef MATCHPATVELORL0_H 
#define MATCHPATVELORL0_H 1

// Include files
// from Gaudi



#include "HltCommon/HltAlgorithm.h"
#include "HltCommon/HltFunctions.h"


// from PatTools
#include "PatTools/PatDataStore.h"

#include "Event/L0MuonCandidate.h"

#include "TrackInterfaces/ITrackSelector.h"
#include "MuonTools/IMuonPosTool.h"
// local



/** @class MatchPatVeloRL0 MatchPatVeloRL0.h
 *  modified from L1Match2dL0 by Mariusz Witek to adapt to the track event model
 *  to read RTTC data
 *  @author Sandra Amato
 *  @date   2003-12-14
 */
class MatchPatVeloRL0 : public HltAlgorithm {
public: 
  /// Standard constructor
  MatchPatVeloRL0( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~MatchPatVeloRL0( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize   ();    ///< Algorithm execution
  
   double eCorMuon(const int&  istation,
  		  const double &e,
		    double &de);
  
 protected:
  
  bool acceptTrack(const LHCb::Track& track);
  
 private:
  
  double m_phiZone[8];    ///< phi of VELO zones

  StatusCode match2dMuon(LHCb::Track* ptr, float & chi);
  
  
  //TrgL0MuonProvider*  m_l0MuonProvider;///< Pointer to the muons container tool
  //TrgL0MuonContainer* m_inputL0Muons;	///< Pointer to the L0 muons
  
  double  m_ptkickConstant;
  double  m_m1EcorParams0;
  double  m_m1EcorParams1;
  double  m_m1EcorParams2;
  double  m_m1EresParams0;
  double  m_m1EresParams1;
  double  m_nom1EcorParams0;
  double  m_nom1EcorParams1;
  double  m_nom1EcorParams2;
  double  m_nom1EresParams0;
  double  m_nom1EresParams1;
  double  m_maxChi2MatchMuon2d; ///< Maximum chi2 to accept matching
  
  double m_sqrt12;
  //bool m_level1;                      ///< specifies if running in L1 mode.
  //bool m_onTES;                       ///< Are tracks on TES ?
  //std::string m_inputTracksName;
  //std::string m_trackSelectorName; /// name of the tool to accept tracks
  
  //  ITrackSelector* m_trackSelector; /// pointer to the tool to accept tracks
  
  
  IMuonPosTool      *m_iPosTool;

  LHCb::L0MuonCandidates* m_inputL0Muon;
  HltHisto m_histoChi;


};
#endif // MATCHPATVELORL0_H
