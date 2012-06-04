// $Id: $
#ifndef TORCHTESTBEAMANALYSIS_TORCHTBHITANALYSIS_H 
#define TORCHTESTBEAMANALYSIS_TORCHTBHITANALYSIS_H 1

// Include files

#include "TorchTBG4FillHistoSet6.h" 

/** @class TorchTBHitAnalysis TorchTBHitAnalysis.h TorchTestBeamAnalysis/TorchTBHitAnalysis.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-06-01
 */
class TorchTBHitAnalysis {
public: 
  /// Standard constructor
  TorchTBHitAnalysis( ); 

  virtual ~TorchTBHitAnalysis( ); ///< Destructor

  void CountNumHits(const G4Event* anEvent, const int RichG4CollectionID);
  void setTorchTBHisto(  TorchTBG4FillHistoSet6* aH){ m_TorchTBHisto=aH;}
  
protected:

private:

  TorchTBG4FillHistoSet6* m_TorchTBHisto;
  
};
#endif // TORCHTESTBEAMANALYSIS_TORCHTBHITANALYSIS_H
