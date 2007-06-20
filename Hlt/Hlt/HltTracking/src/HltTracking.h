// $Id: HltTracking.h,v 1.6 2007-06-20 12:22:43 hernando Exp $
#ifndef HLTTRACKING_H 
#define HLTTRACKING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/ISequencerTimerTool.h"
#include "PatTools/PatDataStore.h"
#include "HltBase/IHltDataStore.h"
#include "HltBase/HltFunctions.h"
#include "HltBase/HltAlgorithm.h"

/** @class HltTracking HltTracking.h
 *  
 *  functionality:
 *        master algorithms to steer tracks reconstraction of HLT
 *        do reconstruct only selected tracks
 *        if they were already reconstructed, do not do it again
 *        it can order output tracks by Pt
 *        it can store output tracks in HltDataStore.
 *
 *  Options:
 *        RecoName: name of the reconstruction to be done,
 *           valid names: "Velo","VeloTT","Forward","MuonForward"
 *        OrderByPt: true/false order by Pt output tracks in HltDataStore
 *
 *  Note:
 *        HltTracking will flag tracks to reconstruct with the IPSelected flag
 *        first it will check that the tracks are not already reconstructed
 *        The containers that own the tracks are the default container in
 *     PatDataStore, you can see the code in the constructor.
 *        If not outputTrackName is used in options, tracks will be only in the
 *     PatDataStore
 *        If OutTracksName is indicated in the options, they will be
 *     a collection in HltDataStore with that name
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-08-28
 */
class HltTracking : public HltAlgorithm {
public: 
  /// Standard constructor
  HltTracking( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTracking( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  StatusCode iniRecoAlgorithm();
  bool isAncestor(const LHCb::Track& mother, const LHCb::Track& son);

  void flag();
  void load();

protected:

  class RecoConfiguration 
  {
  public:
    RecoConfiguration(){}
    RecoConfiguration(const std::string vreco,
                      const std::string vprevreco,
                      const std::string vtype,
                      const std::string vinputTracks,
                      const std::string voutputTracks):
      reco(vreco),prevreco(vprevreco),type(vtype),
      inputTracksName(vinputTracks),outputTracksName(voutputTracks) {};
    ~RecoConfiguration() {}
    std::string reco;
    std::string prevreco;
    std::string type;
    std::string inputTracksName;
    std::string outputTracksName;
  };
  
  std::map< std::string, RecoConfiguration > m_configs;

protected:

  std::string m_recoName;
  bool m_orderByPt;
  bool m_transferExtraInfo;

  int m_recoKey;
  int m_prevrecoKey;
  int m_recoHasPt;

  bool m_doRest;

  PatTrackContainer* m_patOutputTracks;

protected:

  bool m_measureTime;

  ISequencerTimerTool* m_timer;
  int m_timeFlag;
  int m_timePat;
  int m_timeLoad;

protected:

  bool isReco(const LHCb::Track& track) 
  {return (track.info(m_recoKey,0) != 0);}

  void setReco(LHCb::Track& track) 
  { track.addInfo(m_recoKey,1);}
  
  void loadFrom(const LHCb::Track& mother);

  template <class CON>
  void markTracks(CON& con) {
    int nf=0; int nr=0;
    for (typename CON::iterator it = con.begin(); it != con.end(); ++it) {
      if  (isReco(**it)) { loadFrom( **it ); nr+=1; }
      else { (*it)->setFlag(LHCb::Track::IPSelected,true); nf+=1;}
    }
    debug() << " flag tracks " << nf << " retrieved " << nr << endreq;
  }
  
  
  template <class CON>
  void loadTracks(CON& con) {
    if (!m_outputTracks) return;
    for (typename CON::iterator it = con.begin(); it != con.end(); ++it) 
      if ( (*it)->checkFlag(LHCb::Track::IPSelected)) 
      {setReco(**it); loadFrom(**it);}
  }

private:

  HltHisto m_histoPatOutputTracks;

  IAlgorithm* m_algo;
  Hlt::SortTrackByPt _sortByPt;
};
#endif // HLTTRACKING_H
