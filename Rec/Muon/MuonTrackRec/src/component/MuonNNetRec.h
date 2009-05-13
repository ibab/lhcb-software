// $Id: MuonNNetRec.h,v 1.2 2009-05-13 09:53:29 ggiacomo Exp $
#ifndef MUONNNETREC_H 
#define MUONNNETREC_H 1

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "MuonTrackRec/IMuonTrackRec.h"            // Interface
#include "MuonTrackRec/MuonTrack.h"
#include "MuonTrackRec/MuonNeuron.h"
#include <string>
#include <vector>
class DeMuonDetector;
class IMuonHitDecode;
class IMuonPadRec;
class IMuonFastPosTool;
class ISequencerTimerTool;

/** @class MuonNNetRec MuonNNetRec.h
 *  
 *
 *  @author Giovanni Passaleva 
 *  @date   2008-04-11
 */
class MuonNNetRec : public GaudiTool, virtual public IMuonTrackRec, 
                      virtual public IIncidentListener {
public: 
  /// Standard constructor
  MuonNNetRec( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~MuonNNetRec( ); ///< Destructor
  // from GaudiTool
  virtual StatusCode initialize();    
  virtual StatusCode finalize  ();    
  // from IIncidentListener
  virtual void handle ( const Incident& incident );   

  //------------------
  virtual inline const std::vector<MuonHit*>* trackhits()   {
    if(!m_recDone) muonNNetMon();
    return (const std::vector<MuonHit*>*) (&m_trackhits);
  }
  virtual inline const std::vector<MuonTrack*>* tracks()   {
    if(!m_recDone) muonNNetMon();
    return (const std::vector<MuonTrack*>*) (&m_tracks);
  }
  virtual inline const std::vector<MuonNeuron*>* useneurons()   {
    if(!m_recDone) muonNNetMon();
    return (const std::vector<MuonNeuron*>*) (&m_useneurons);
  }
  virtual inline const std::vector<MuonNeuron*>* allneurons()   {
    if(!m_recDone) muonNNetMon();
    return (const std::vector<MuonNeuron*>*) (&m_allneurons);
  }
  virtual inline bool recOK() {return (m_recDone && m_recOK);}
  virtual inline bool tooManyHits() {return m_tooManyHits;}
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
  virtual void setOfflineTimeAlignment(bool OfflineTimeAlign) 
  { MuonTrackRec::OfflineTimeAlign = OfflineTimeAlign;}
  virtual void setTimeResidualMap(std::map<long int, float>* Map) 
  { MuonTrackRec::ResMap = Map;}

private:
  IMuonHitDecode* m_decTool;
  IMuonPadRec* m_padTool;
  IMuonFastPosTool* m_posTool;
  DeMuonDetector* m_muonDetector;
  bool m_recDone;
  bool m_recOK;
  bool m_tooManyHits;
  bool m_forceResetDAQ;

  std::vector< MuonHit* > m_trackhits;
  std::vector< MuonNeuron* > m_useneurons;
  std::vector< MuonNeuron* > m_allneurons;
  std::vector< MuonTrack* > m_tracks;

  void clear();

  StatusCode muonNNetMon();
  StatusCode trackFit();
  bool loadTimeRes(); 

   // algorithm timing monitor
  ISequencerTimerTool* m_timer;
  int m_timeinitNet;
  int m_timeconvNet;
  int m_timefitTrack;
  int m_timeTile2xyz;

  // job options
  double m_aa;    // head-tail weight scale factor
  double m_bb;    // head-head weight scale factor
  double m_cc;    // tail-tail weight scale factor
  double m_slamb; // penalty for TT connections
  double m_slamc; // penalty for HH connections
  double m_xesp1; // exponent for (1-sin(thxz)) weight factor
  double m_xesp2; // exponent for (1-sin(thyz)) weight factor
  double m_dd;    // stimulation weight term
  double m_temp;  // temperature
  double m_dsum;  // convergence factor
  double m_scut;  // neuron activation cut
  double m_acut;  // angular cut for double length neurons killing
  double m_span_cut;  // cut on span for selected tracks
  double m_firing_cut; // min # of stations firing in the track
  int m_maxNeurons; // max number of possible track segments
  int m_maxIterations; // max number of NN iterations
  // station to be skipped (e.g. for eff. study): 0-4;
  // -1 = keep all stns (default)
  int m_skipStation;
  // if <true> (default) holes of 1 station are allowed in track reconstruction
  bool m_allowHoles;
  // if true, we assume that timing is the final one (accounting for TOF from primary vx) 
  bool m_physicsTiming;
  // if true (default) we assume that tracks are of cosmic origin (can be backward)
  bool m_assumeCosmics;
  // if true we assume that tracks have the "right" direction (pz>0)
  bool m_assumePhysics;
  // if true, offline-computed corrections are applied to detector time-alignment
  bool m_offlineTimeAlignment;
  // path of file containing offline-computed corrections to time-alignment
  std::string m_timeResidualFile;
  // name of decoding tool (MuonHitDecode for offline, MuonMonHitDecode for online monitoring)
  std::string m_decToolName;
  // name of pad rec tool (MuonPadRec only option so far)
  std::string m_padToolName;
  // name of tile 2 coordinate conversion tool (MuonDetPosTool for using MuonDet, faster alternative provided by HltMuon)
  std::string m_posToolName;
  /// cross talk
  bool m_XTalk;
    
};
#endif // MUONNNETREC_H
