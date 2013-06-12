#ifndef CALONTPBASE_H 
#define CALONTPBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"
#include "GaudiKernel/IEventTimeDecoder.h"
#include "CaloUtils/CaloMomentum.h"
#include "CaloUtils/ICaloElectron.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/ProtoParticle.h"
#include "GaudiKernel/IEventTimeDecoder.h"
#include "Event/ODIN.h" 

/** @class CaloNtpBase CaloNtpBase.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2010-10-12
 */
class CaloNtpBase : public GaudiTupleAlg {
public: 
  /// Standard constructor
  CaloNtpBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloNtpBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization

  ICaloHypo2Calo* toSpd(){return m_toSpd;} ;   
  ICaloHypo2Calo* toPrs(){return m_toPrs;} ;
  DeCalorimeter* calo(){return m_calo;};
  bool inRange( std::pair<double,double> range, double value){
    return ( value >= range.first) && (value <= range.second);
  }  
  bool inRange( std::pair<int,int> range, int value){
    return ( value == 0 &&   range.first == 1 ) || (value > 0 && range.second == 1);
  }

  unsigned int nSpd(){
    std::string loc =   LHCb::CaloAlgUtils::CaloDigitLocation( "SPD" );
    if( !exist<LHCb::CaloDigits>(loc) )return 0;
    const LHCb::CaloDigits* digits = get<LHCb::CaloDigits> (loc );
    return (NULL != digits) ? digits->size() : 0;
  };
  unsigned int nTracks(){ 
    std::string loc =  LHCb::TrackLocation::Default;
    if( !exist<LHCb::Tracks>(loc) )return 0;
    LHCb::Tracks* tracks= get<LHCb::Tracks>(loc);
    return (NULL != tracks) ? tracks->size() : 0;
  }
  unsigned int nVertices();
  ITrackExtrapolator* extrapolator(){return m_extrapolator;};
  ICaloElectron* caloElectron(){return m_caloElectron;}
  bool acceptTrack(const LHCb::Track*);
  bool eventProcessing();
  bool hypoProcessing(const LHCb::CaloHypo* hypo);
  bool inArea(const LHCb::CaloHypo* hypo);  

protected:

  bool m_histo;
  bool m_tuple;
  bool m_profil;
  bool m_trend;
  bool m_inArea;

  std::string m_vertLoc;
  bool m_usePV3D;
  int m_run;
  ulonglong m_evt;
  int m_tty ;
  std::vector<int> m_tracks;
  std::string m_extrapolatorType;
  // global cuts
  std::pair<double,double> m_nTrk;
  std::pair<double,double> m_nSpd;
  std::pair<double,double> m_nVtx;
  // hypo cuts
  std::pair<double,double> m_e;
  std::pair<double,double> m_et;
  std::pair<double,double> m_prs;
  std::pair<double,double> m_spd;
  
  std::string m_input;


private:
  ITrackExtrapolator*  m_extrapolator;
  DeCalorimeter* m_calo;
  ICaloElectron* m_caloElectron;
  ICaloHypo2Calo* m_toSpd ;   
  ICaloHypo2Calo* m_toPrs ;   
  IEventTimeDecoder* m_odin;

};
#endif // CALONTPBASE_H
