// $Id: RichG4HistoFill.h,v 1.1 2003-04-29 15:04:54 seaso Exp $
#ifndef RICHANALYSIS_RICHG4HISTOFILL_H 
#define RICHANALYSIS_RICHG4HISTOFILL_H 1

// Include files
#include "G4Event.hh"
#include <vector>

/** @class RichG4HistoFill RichG4HistoFill.h RichAnalysis/RichG4HistoFill.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2003-03-20
 */
class RichG4HistoFill {
public:
  /// Standard constructor
  RichG4HistoFill( ); 

  virtual ~RichG4HistoFill( ); ///< Destructor
 
  void FillRichG4Histo( const G4Event* anEvent,
           int NumRichColl, std::vector<int>RichG4CollectionID );
  
protected:

private:

};
#endif // RICHANALYSIS_RICHG4HISTOFILL_H
