// $Id: RichG4HistoFillSet2.h,v 1.3 2008-01-21 16:55:34 seaso Exp $
#ifndef RICHANALYSIS_RICHG4HISTOFILLSET2_H
#define RICHANALYSIS_RICHG4HISTOFILLSET2_H 1

// Include files
#include "G4Event.hh"
#include <vector>

/** @class RichG4HistoFillSet2 RichG4HistoFillSet2.h
 *    RichAnalysis/RichG4HistoFillSet2.h
 *
 *
 *  @author Sajan EASO
 *  @date   2003-03-20
 */

class RichG4HistoFillSet2 {

public:

  /// Standard constructor
  RichG4HistoFillSet2( );

  virtual ~RichG4HistoFillSet2( ); ///< Destructor

  void FillRichG4HistoSet2( const G4Event* anEvent,
                            int NumRichColl,
                            const std::vector<int> & RichG4CollectionID );
  void FillRichG4HistoSet2A( const G4Event* anEvent,
                             int NumRichColl,
                             const std::vector<int> & RichG4CollectionID );
  void FillRichG4HistoSet2B( const G4Event* anEvent,
                             int NumRichColl,
                             const std::vector<int> & RichG4CollectionID );

protected:

private:

};
#endif // RICHANALYSIS_RICHG4HISTOFILLSET2_H
