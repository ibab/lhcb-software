// $Id: RichG4HistoFillSet3.h,v 1.3 2009-07-03 11:59:49 seaso Exp $
#ifndef GAUSSRICH_RICHG4HISTOFILLSET3_H 
#define GAUSSRICH_RICHG4HISTOFILLSET3_H 1

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
 
  void FillRichG4HistoSet3();
  
  
protected:

private:

};
#endif // GAUSSRICH_RICHG4HISTOFILLSET3_H
