// $Id: RichG4HitRecon.h,v 1.2 2004-02-10 14:24:13 jonesc Exp $
#ifndef RICHANALYSIS_RICHG4HITRECON_H
#define RICHANALYSIS_RICHG4HITRECON_H 1

// Include files

#include "RichG4CkvRecon.h"
#include "RichG4ReconFlatMirr.h"
#include "RichG4HistoFillSet4.h"

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

  void setSatHitUse( bool useSaturatedHit )
  {
    m_useOnlySaturatedHits = useSaturatedHit;

  }
  void setMidRadiatorUse( bool useMidRadiatorEmis )
  {
    m_useMidRadiatorZEmisPt = useMidRadiatorEmis;

  }


protected:

private:

  RichG4CkvRecon* m_RichG4CkvRec;
  RichG4ReconFlatMirr* m_RichG4ReconFlatMirr;
  RichG4HistoFillSet4* m_RichG4HistoFillSet4Ckv;
  RichG4ReconResult* m_RichG4ReconResult;

  double m_agelnominalrefindex;
  double m_c4f10nominalrefindex;
  double m_MidRich1AgelZ;
  double m_MidRich1GasZ;
  double m_MidRich2GasZ;

  bool m_useOnlySaturatedHits;
  bool m_useMidRadiatorZEmisPt;
  double m_chtkBetaSaturatedCut;


};
#endif // RICHANALYSIS_RICHG4HITRECON_H
