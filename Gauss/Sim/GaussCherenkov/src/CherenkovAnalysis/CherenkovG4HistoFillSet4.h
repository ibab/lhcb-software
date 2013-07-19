// $Id: RichG4HistoFillSet4.h,v 1.2 2004-02-10 14:24:12 jonesc Exp $
#ifndef RICHANALYSIS_CHERENKOVG4HISTOFILLSET4_H
#define RICHANALYSIS_CHERENKOVG4HISTOFILLSET4_H 1

// Include files
// Include files
#include "G4Event.hh"
#include <vector>

#include "GaussCherenkov/CkvG4Hit.h"
#include "GaussRICH/RichG4ReconResult.h"
#include "GaussRICH/RichG4HitCoordResult.h"

/** @class CherenkovG4HistoFillSet4 CherenkovG4HistoFillSet4.h RichAnalysis/CherenkovG4HistoFillSet4.h
 *
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-22
 */
class CherenkovG4HistoFillSet4 {
public:
  /// Standard constructor
  CherenkovG4HistoFillSet4( );

  virtual ~CherenkovG4HistoFillSet4( ); ///< Destructor

  void FillRichG4HistoSet4(CkvG4Hit* acHit,
                           RichG4ReconResult* aRichG4ReconResult );

  void FillRichG4CoordHistoSet4(CkvG4Hit* adHit, 
                                RichG4HitCoordResult* bReconCoordResult) ;
  

protected:

private:

};
#endif // RICHANALYSIS_RICHG4HISTOFILLSET4_H
