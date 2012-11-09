// $Id: RichG4HitRecon.h,v 1.4 2006-02-27 14:10:30 seaso Exp $
#ifndef RICHANALYSIS_RICHG4HITRECON_H
#define RICHANALYSIS_RICHG4HITRECON_H 1

// Include files

#include "RichG4CkvRecon.h"
#include "GaussRICH/RichG4ReconFlatMirr.h"
#include "RichG4HistoFillSet4.h"
#include "RichG4HistoFillSet5.h"
#include "GaussRICH/RichG4HitCoordResult.h"

#include "G4Event.hh"

/** @class RichG4HitRecon RichG4HitRecon.h RichAnalysis/RichG4HitRecon.h
 *
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-11
 */
class RichG4HitRecon {

public:

  RichG4HitRecon( );
  virtual ~RichG4HitRecon( ); ///< Destructor

  void RichG4ReconstructCherenkovAngle(const G4Event* anEvent,
                                       int NumRichColl, 
                                       const std::vector<int> & RichG4CollectionID ) ;

  void RichG4GetHpdOccupancies(const G4Event* anEvent,
                                       int NumRichColl, 
                                       const std::vector<int> & RichG4CollectionID ) ;

  void setRichG4CkvRec ();
  RichG4CkvRecon* getRichG4CkvRec()
  {
    return m_RichG4CkvRec;
  }

  void   setRichG4FlatMirr();
  RichG4ReconFlatMirr* getRichG4ReconFlatMirr();

  RichG4HistoFillSet4* RichG4HistoFillSet4Ckv()
  {
    return m_RichG4HistoFillSet4Ckv;

  }

  RichG4ReconResult* getRichG4ReconResult ()
  {
    return m_RichG4ReconResult;

  }
  void setRichG4ReconResult( RichG4ReconResult* aRichG4ReconResult )
  {
    m_RichG4ReconResult = aRichG4ReconResult;

  }

  void setRichG4HistoFillSet4Ckv(RichG4HistoFillSet4* aRichG4HistoFillSet4 )
  {
    m_RichG4HistoFillSet4Ckv = aRichG4HistoFillSet4;

  }

  void setRichG4HistoFillSet5Occp(RichG4HistoFillSet5* aRichG4HistoFillSet5 )
  {
    m_RichG4HistoFillSet5Occp = aRichG4HistoFillSet5;

  }

  void setSatHitUse( bool useSaturatedHit )
  {
    m_useOnlySaturatedHits = useSaturatedHit;

  }
  void setMidRadiatorUse( bool useMidRadiatorEmis )
  {
    m_useMidRadiatorZEmisPt = useMidRadiatorEmis;

  }
  RichG4HitCoordResult* getRichG4HitCoordResult() 
  {
    return m_RichG4HitCoordResult;
  }
  
  void setRichG4HitCoordResult(RichG4HitCoordResult* ar ) 
  {m_RichG4HitCoordResult= ar;}
  
  bool  useOnlyStdRadiatorHits() {
    return m_useOnlyStdRadiatorHits;
  } 
  void setuseOnlyStdRadiatorHits( bool afl ) {
    m_useOnlyStdRadiatorHits= afl;
  }
  bool useOnlySignalHitsInRecon() 
  {return m_useOnlySignalHitsInRecon;}
  void setuseOnlySignalHitsInRecon(bool arf) {
   m_useOnlySignalHitsInRecon=arf;
  }
  void setactivateMinMomForTrackRecon(bool ah ) 
  {m_activateMinMomForTrackRecon=ah;  }
  
protected:

private:

  RichG4CkvRecon* m_RichG4CkvRec;
  RichG4ReconFlatMirr* m_RichG4ReconFlatMirr;
  RichG4HistoFillSet4* m_RichG4HistoFillSet4Ckv;
  RichG4HistoFillSet5* m_RichG4HistoFillSet5Occp;

  RichG4ReconResult* m_RichG4ReconResult;
  RichG4HitCoordResult* m_RichG4HitCoordResult;
  
  double m_agelnominalrefindex;
  double m_c4f10nominalrefindex;
  double m_MidRich1AgelZ;
  double m_MidRich1GasZ;
  double m_MidRich2GasZ;

  bool m_useOnlySaturatedHits;
  bool m_useMidRadiatorZEmisPt;
  double m_chtkBetaSaturatedCut;
  bool m_useOnlyStdRadiatorHits;
  bool m_useOnlySignalHitsInRecon;
  bool m_activateMinMomForTrackRecon;
  double m_minMomTracksForReconR1Gas;
  double m_minMomTracksForReconR2Gas;
  double m_minMomTracksForReconAerogel;
  
};
#endif // RICHANALYSIS_RICHG4HITRECON_H
