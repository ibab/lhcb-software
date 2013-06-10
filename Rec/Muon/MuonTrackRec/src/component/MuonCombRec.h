#ifndef COMPONENT_MUONCOMBREC_H 
#define COMPONENT_MUONCOMBREC_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "MuonInterfaces/IMuonTrackRec.h"            // Interface
#include "MuonInterfaces/MuonTrack.h"
#include "MuonInterfaces/MuonNeuron.h"

// forward declarations
class DeMuonDetector;
class IMuonHitDecode;
class IMuonPadRec;
class IMuonClusterRec;
class ISequencerTimerTool;
class IMuonTrackMomRec;
/** @class MuonCombRec MuonCombRec.h component/MuonCombRec.h
 *  
 *
 *  @author Giovanni Passaleva
 *  @date   2009-10-07
 */
class MuonCombRec : public GaudiTool, virtual public IMuonTrackRec, 
                      virtual public IIncidentListener {
public: 
  /// Standard constructor
  MuonCombRec( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~MuonCombRec( ); ///< Destructor
  // from GaudiTool
  virtual StatusCode initialize();    
  virtual StatusCode finalize  ();    
  // from IIncidentListener
  virtual void handle ( const Incident& incident );   

  // implementation of interface methods
  virtual inline const std::vector<MuonHit*>* trackhits()   {
    if(!m_recDone) muonTrackFind();
    return (const std::vector<MuonHit*>*) (&m_trackhits);
  }

  virtual inline const std::vector<MuonTrack*>* tracks()   {
    if(!m_recDone) muonTrackFind();
    return (const std::vector<MuonTrack*>*) (&m_tracks);
  }
  virtual inline const std::vector<MuonNeuron*>* useneurons()   {
    return NULL;
  }
  virtual inline const std::vector<MuonNeuron*>* allneurons()   {
    return NULL;
  }
  virtual inline bool recOK() {return (m_recDone && m_recOK);}
  virtual inline bool tooManyHits() {return false;}
  virtual inline bool clusteringOn() {return (m_clusterToolName != "MuonFakeClustering");}
  virtual inline void setZref(double Zref) { MuonTrackRec::Zref=Zref; }
  virtual void setPhysicsTiming(bool PhysTiming) { MuonTrackRec::PhysTiming = PhysTiming;}
  virtual void setAssumeCosmics(bool AssumeCosmics) { 
    MuonTrackRec::IsCosmic = AssumeCosmics;
    if (AssumeCosmics) MuonTrackRec::IsPhysics = false;
  }
  virtual void setAssumePhysics(bool AssumePhysics) { 
    MuonTrackRec::IsPhysics = AssumePhysics;
    if(AssumePhysics) MuonTrackRec::IsCosmic = false;
  }
  virtual void setSeedStation(int seedS) 
  { m_seedStation = seedS;}
  virtual void setSkipStation(int skipS) 
  { 
    if( skipS >= 0) {
      m_recDone = false; // reset the recDone flag to allow a new track reconstruction
      m_cloneKiller = false; // do not kill clones
      m_strongCloneKiller = false;  // do not kill clones
      m_skipStation = skipS; // set the station to be skipped
      if(m_skipStation == 4) { // special treatment for M5
        setSeedStation(3);
      }
    } else {
      m_skipStation = skipS; // set the station to be skipped
      m_recDone = false; // reset the recDone flag to allow a new track reconstruction
      m_cloneKiller = m_optCloneKiller;
      m_strongCloneKiller = m_optStrongCloneKiller;
      if( m_strongCloneKiller ) m_cloneKiller = false; // don't run both

      // In case the seed station is M5 the clone killing does not work
      // FIXME: Make the clone killing more flexible
      if( m_seedStation != M5 ) {
        m_cloneKiller = false;
        m_strongCloneKiller = false;
      }
    }
  }
  virtual StatusCode copyToLHCbTracks();

private:

  // station mnemonics
  enum{M1=0, // M1
       M2,   // M2
       M3,   // M3
       M4,   // M4
       M5    // M5
         };
  // region mnemonics
  enum{R1=0, // R1
       R2,   // R2
       R3,   // R3
       R4    // R4
         };
 
  IMuonHitDecode* m_decTool;
  IMuonPadRec* m_padTool;
  IMuonClusterRec* m_clusterTool;
  IMuonTrackMomRec* m_momentumTool;


  //  IMuonFastPosTool* m_posTool;
  DeMuonDetector* m_muonDetector;
  bool m_recDone;
  bool m_recOK;
  bool m_hitsDone; // MuonHit container filled
  bool m_sortDone; // MuonHit-s sorted
  //  bool m_tooManyHits;

  std::vector<float> m_xFOIs;
  std::vector<float> m_yFOIs;

  float m_zStations[5];
  
  std::vector< MuonHit* > m_trackhits;
  std::vector< MuonTrack* > m_tracks;
  std::vector< std::vector<MuonHit*>* > m_sortedHits;

  void clear();

  int m_nStation;
  int m_nRegion;

  ISequencerTimerTool* m_timer;
  int m_timeLoad;
  int m_timePad;
  int m_timeMuon;              
  //  int m_timeTag;
  int m_timeMuonStore;

  // pad sizes are hardwired to speed up the algorithm
  std::vector<double> m_pad_x;
  std::vector<double> m_pad_y;

  
  // counters
  //  int m_countEvents;
  //  int m_countMuCandidates;

  // properties


  // maximum number of tracks axcepted
  //  int m_maxMuonFound;  
  // enable timers
  bool m_measureTime;  
  // enable clone finding and killing
  bool m_optCloneKiller;
  bool m_cloneKiller;
  // enable strong clone fining and killing
  bool m_optStrongCloneKiller;
  bool m_strongCloneKiller;

  // station to be skipped (e.g. for eff. study): 0-4;
  // -1 = keep all stns (default)
  int m_optSkipStation;
  int m_skipStation;
  // if true, we assume that timing is the final one (accounting for TOF from primary vx) 
  bool m_physicsTiming;
  // if true (default) we assume that tracks are of cosmic origin (can be backward)
  bool m_assumeCosmics;
  // if true we assume that tracks have the "right" direction (pz>0)
  bool m_assumePhysics;
  // name of decoding tool (MuonHitDecode for offline, MuonMonHitDecode for online monitoring)
  std::string m_decToolName;
  // name of pad rec tool (MuonPadRec only option so far)
  std::string m_padToolName;
  // name of clustering tool
  std::string m_clusterToolName;
  // name of tile 2 coordinate conversion tool (MuonDetPosTool for using MuonDet, faster alternative provided by HltMuon)
  //  std::string m_posToolName;
  /// cross talk
  bool m_XTalk;
  /// station used for seed
  int m_optSeedStation;
  int m_seedStation;
  /// LHCb tracks output location in TES
  std::string m_trackOutputLoc ;

  // main steering reconstruction routine
  StatusCode muonTrackFind();

  StatusCode findMatching( double x, double y,
                           int lookInStation,
                           int lookInRegion,
                           std::vector<MuonHit*> &Vec_candidate );

  StatusCode sortMuonHits();
  StatusCode muonSearch();
  StatusCode cloneKiller();    
  StatusCode strongCloneKiller();
  StatusCode trackFit();

};
#endif // COMPONENT_MUONCOMBREC_H
