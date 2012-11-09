// $Id: RichG4HitRecon.h,v 1.4 2006-02-27 14:10:30 seaso Exp $
#ifndef RICHANALYSIS_CHERENKOVG4HITRECON_H
#define RICHANALYSIS_CHERENKOVG4HITRECON_H 1

// Include files

#include "CherenkovG4CkvRecon.h"
#include "GaussRICH/RichG4ReconFlatMirr.h"
#include "CherenkovG4HistoFillSet4.h"
#include "CherenkovG4HistoFillSet5.h"
#include "GaussRICH/RichG4HitCoordResult.h"

#include "G4Event.hh"

/** @class CherenkovG4HitRecon CherenkovG4HitRecon.h RichAnalysis/CherenkovG4HitRecon.h
 *
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-11
 */
class CherenkovG4HitRecon {

public:

  CherenkovG4HitRecon( );
  virtual ~CherenkovG4HitRecon( ); ///< Destructor

  void RichG4ReconstructCherenkovAngle(const G4Event* anEvent,
                                       int NumRichColl, 
                                       const std::vector<int> & RichG4CollectionID ) ;


  void RichG4GetOccupancies(const G4Event* anEvent,int NumRichColl,
                            const std::vector<int> & RichG4CollectionID ) ;

  void setRichG4CkvRec ();
  CherenkovG4CkvRecon* getRichG4CkvRec()
  {
    return m_RichG4CkvRec;
  }

  void   setRichG4FlatMirr();
  RichG4ReconFlatMirr* getRichG4ReconFlatMirr();

  CherenkovG4HistoFillSet4* RichG4HistoFillSet4Ckv()
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

  void setRichG4HistoFillSet4Ckv(CherenkovG4HistoFillSet4* aRichG4HistoFillSet4 )
  {
    m_RichG4HistoFillSet4Ckv = aRichG4HistoFillSet4;

  }
  void setCherenkovG4HistoFillSet5Occp(CherenkovG4HistoFillSet5* aRichG4HistoFillSet5Occp )
  {
   m_CherenkovG4HistoFillSet5Occp = aRichG4HistoFillSet5Occp ;

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
  void setuseOnlySignalHitsInRecon(bool arf) {
   m_useOnlySignalHitsInRecon=arf;
  }
   void setactivateMinMomForTrackRecon(bool ah ) 
  {m_activateMinMomForTrackRecon=ah;  }
 protected:

private:

  CherenkovG4CkvRecon* m_RichG4CkvRec;
  RichG4ReconFlatMirr* m_RichG4ReconFlatMirr;
  CherenkovG4HistoFillSet4* m_RichG4HistoFillSet4Ckv;
  CherenkovG4HistoFillSet5* m_CherenkovG4HistoFillSet5Occp;
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
 
};
#endif // RICHANALYSIS_CHERENKOVG4HITRECON_H
