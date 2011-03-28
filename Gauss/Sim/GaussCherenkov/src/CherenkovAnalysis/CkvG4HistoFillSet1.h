// $Id: RichG4HistoFillSet1.h,v 1.2 2004-02-10 14:24:10 jonesc Exp $
#ifndef RICHANALYSIS_CKVG4HISTOFILLSET1_H 
#define RICHANALYSIS_CKVG4HISTOFILLSET1_H 1

// Include files
#include "G4Event.hh"
#include <vector>

/** @class CkvG4HistoFillSet1 CkvG4HistoFillSet1.h 
 *    RichAnalysis/CkvG4HistoFillSet1.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2003-03-20
 */
class CkvG4HistoFillSet1 {
public:
  /// Standard constructor
  CkvG4HistoFillSet1( ); 

  virtual ~CkvG4HistoFillSet1( ); ///< Destructor
 
  void FillRichG4HistoSet1( const G4Event* anEvent,
                            int NumRichColl, 
                            const std::vector<int> & RichG4CollectionID );
  
protected:

private:

};
#endif // RICHANALYSIS_CKVG4HISTOFILLSET1_H
