// $Id: RichG4EventHitCount.h,v 1.1 2003-07-16 13:24:06 seaso Exp $
#ifndef RICHANALYSIS_RICHG4EVENTHITCOUNT_H 
#define RICHANALYSIS_RICHG4EVENTHITCOUNT_H 1

// Include files
// Include files
#include "G4Event.hh"
#include <vector>
#include "globals.hh"
/** @class RichG4EventHitCount RichG4EventHitCount.h 
 * RichAnalysis/RichG4EventHitCount.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2003-06-16
 */

class RichG4EventHitCount {
public:
  /// Standard constructor
  RichG4EventHitCount( ); 

  virtual ~RichG4EventHitCount( ); ///< Destructor

  void RichG4CountAndClassifyHits( const G4Event* anEvent,
                      int NumRichColl,
       std::vector<int> RichG4CollectionID ) ;

  std::vector<int> RichG4GetChTrajId(const G4Event* anEvent);

  bool Rich1TrajTraverse(const G4Event* anEvent, int trajId);
  bool Rich2TrajTraverse(const G4Event* anEvent, int trajId);
  
   void RichG4CountSaturatedHits( const G4Event* anEvent,
                      int NumRichColl,
       std::vector<int> RichG4CollectionID ) ;
   
protected:

private:

  G4double ChTkBetaSaturatedCut;
  
};
#endif // RICHANALYSIS_RICHG4EVENTHITCOUNT_H
