// $Id: RichG4HistoFillSet1.h,v 1.1 2003-04-29 17:08:05 seaso Exp $
#ifndef RICHANALYSIS_RICHG4HISTOFILLSET1_H 
#define RICHANALYSIS_RICHG4HISTOFILLSET1_H 1

// Include files
#include "G4Event.hh"
#include <vector>

/** @class RichG4HistoFillSet1 RichG4HistoFillSet1.h 
 *    RichAnalysis/RichG4HistoFillSet1.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2003-03-20
 */
class RichG4HistoFillSet1 {
public:
  /// Standard constructor
  RichG4HistoFillSet1( ); 

  virtual ~RichG4HistoFillSet1( ); ///< Destructor
 
  void FillRichG4HistoSet1( const G4Event* anEvent,
           int NumRichColl, std::vector<int>RichG4CollectionID );
  
protected:

private:

};
#endif // RICHANALYSIS_RICHG4HISTOFILLSET1_H
