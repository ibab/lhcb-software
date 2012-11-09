// $Id: RichG4MatRadIdentifier.h,v 1.1 2006-02-10 09:34:47 seaso Exp $
#ifndef GAUSSRICH_RICHG4MATRADIDENTIFIER_H 
#define GAUSSRICH_RICHG4MATRADIDENTIFIER_H 1

// Include files
#include "globals.hh"
#include <map>
#include <vector>
/** @class RichG4MatRadIdentifier RichG4MatRadIdentifier.h Misc/RichG4MatRadIdentifier.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2006-02-01
 */
class RichG4MatRadIdentifier {
public: 

  virtual ~RichG4MatRadIdentifier( ); ///< Destructor
  static RichG4MatRadIdentifier* RichG4MatRadIdentifierInstance();
  void InitializeRichG4MatRadIdentifier();
  void InitializeRichCkvMatMaxNumPhot(std::vector<G4int>  aMaxPhotLim );
  G4int RichNumRadMat() {return m_RichNumRadMat; }
  G4int getRadiatorNumForG4MatIndex(G4int cIndex );
  G4int getRadiatorPhotPerStepLimit(G4int kIndex );
  void PrintRadMatIndex ();
  void PrintRadNumPhotStepLim();
  std::map<G4int,G4int> RichG4MatIndexRadNum() {return m_RichG4MatIndexRadNum; }
  std::map<G4int,G4int> RichG4CkvMaxPhotPerStep() {return  m_RichG4CkvMaxPhotPerStep;}

protected:

private:
  // constructor
  RichG4MatRadIdentifier( ); 
  static RichG4MatRadIdentifier* m_RichG4MatRadIdentifierInstance;
  G4int m_RichNumRadMat;                       // number of RICH radiator materials
  std::map<G4int,G4int> m_RichG4MatIndexRadNum; // the map between G4 Matrial Index and radiator numbers.
  std::map<G4int,G4int> m_RichG4CkvMaxPhotPerStep; // max num phot per  cherenkov production step for each material.
};
#endif // GAUSSRICH_RICHG4MATRADIDENTIFIER_H
