// $Id: RichG4HistoFillSet3.h,v 1.1 2003-07-16 13:24:07 seaso Exp $
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
           int NumRichColl, std::vector<int>RichG4CollectionID );
  
protected:

private:

};
#endif // RICHANALYSIS_RICHG4HISTOFILLSET3_H
