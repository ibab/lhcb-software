// $Id: RichG4HistoFillSet3.h,v 1.2 2004-02-10 14:24:11 jonesc Exp $
#ifndef RICHANALYSIS_RICHG4HISTOFILLSET3_H 
#define RICHANALYSIS_RICHG4HISTOFILLSET3_H 1

// Include files
#include "G4Event.hh"
#include <vector>

/** @class RichG4HistoFillSet3 RichG4HistoFillSet3.h 
 *    RichAnalysis/RichG4HistoFillSet3.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2003-03-20
 */
class RichG4HistoFillSet3 {
public:
  /// Standard constructor
  RichG4HistoFillSet3( ); 

  virtual ~RichG4HistoFillSet3( ); ///< Destructor
 
  void FillRichG4HistoSet3( const G4Event* anEvent,
                            int NumRichColl, 
                            const std::vector<int> & RichG4CollectionID );
  
protected:

private:

};
#endif // RICHANALYSIS_RICHG4HISTOFILLSET3_H
