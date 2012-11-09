// $Id: $
#ifndef RICHANALYSIS_RICHG4HISTOFILLSET5_H 
#define RICHANALYSIS_RICHG4HISTOFILLSET5_H 1

// Include files
#include "GaussRICH/RichG4Hit.h"
#include "GaudiKernel/Point3DTypes.h"


/** @class RichG4HistoFillSet5 RichG4HistoFillSet5.h RichAnalysis/RichG4HistoFillSet5.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-03-18
 */
class RichG4HistoFillSet5 {
public: 
  /// Standard constructor
  RichG4HistoFillSet5( ); 

  virtual ~RichG4HistoFillSet5( ); ///< Destructor
  void InitRichG4HistoFillSet5();
  void FillRichG4HistoSet5NumHits();
  void FillRichG4HistoSet5Coordinate(RichG4Hit * aHit, 
                                     Gaudi::XYZPoint aHitCoordInPhDetPlane,
                                     int aTotNumHpdR1);
protected:

private:

  int m_NumHitInRich1InCurEv;
  int m_NumHitInRich2InCurEv;
  int m_NumHitInRich2NoScintInCurEv;
  int m_NumHitInRich1InCurEvBinary;
  int m_NumHitInRich2InCurEvBinary;
  int m_NumHitInRich2NoScintInCurEvBinary;
  

};
#endif // RICHANALYSIS_RICHG4HISTOFILLSET5_H
