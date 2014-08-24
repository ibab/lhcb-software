#ifndef GAUSSCHERENKOV_CKVCOMMONSENSDET_H 
#define GAUSSCHERENKOV_CKVCOMMONSENSDET_H 1

// Include files
// local
#include "GaussCherenkov/CkvG4Hit.h"
#include "GaussRICH/RichG4HitCollName.h"
#include "CkvG4GeomProp.h"
#include <map>

#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "GaudiKernel/StatusCode.h"

/** @class CkvCommonSensDet CkvCommonSensDet.h GaussCherenkov/CkvCommonSensDet.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2014-08-19
 */
class CkvCommonSensDet {
public: 

  virtual ~CkvCommonSensDet( ); ///< Destructor
  static CkvCommonSensDet* getCkvCommonSensDetInstance();

  bool Rich2UseGrandPmt() 
  {    return m_Rich2UseGrandPmt;}

  void InitGeomProp();
  void ResetPmtMapInCurrentEvent();

  StatusCode  ProcessRichPmtHits( G4Step* aStep , G4TouchableHistory* aTh);

  void RichPmtAviodDuplicateHitsActivate(bool adupavoidfl ) 
  {m_RichPmtAviodDuplicateHitsActivate =  adupavoidfl;}
  void RichPmtFlagDuplicateHitsActivate (bool adupfl ) 
  {m_RichPmtFlagDuplicateHitsActivate = adupfl;}
  CkvG4Hit*  newHit()  {  return m_newHit;}
  




  CkvG4GeomProp*  RichGeomProperty() {return  m_RichGeomProperty; }

  G4int NumberofRichDet() {return  m_RichGeomProperty->NumberOfRichDet(); }

  G4int NumberOfDetSectionsInRich1Det() const
  {return m_RichGeomProperty->NumberOfDetSectionsInRich1() ; }

  G4int NumberOfDetSectionsInRich2Det() const
  {return m_RichGeomProperty->NumberOfDetSectionsInRich2(); }

  G4int NumberofPixelsInPMTofRich() const
  {return m_RichGeomProperty->NumberOfPixelsInPMT(); }

  G4double MaxZHitInRich1Detector() const
  {return m_RichGeomProperty->MaxZHitInRich1Det(); }

  const std::vector<G4double> & PixelBoundaryInX() const
  {return m_RichGeomProperty->PixelXBoundary(); }

  const std::vector<G4double> & PixelBoundaryInY() const
  {return m_RichGeomProperty->PixelYBoundary(); }

  G4double PixelBoundaryInXValue(const int aPXNumber ) const
  {return  m_RichGeomProperty->PixelXBoundaryValue(aPXNumber) ;}

  G4double PixelBoundaryInYValue(const int aPYNumber ) const
  {return  m_RichGeomProperty->PixelYBoundaryValue(aPYNumber) ;}

  G4int PixelXNum(const G4double localXCoord ) const
  {return m_RichGeomProperty->PixelXNumFromCoord(localXCoord) ;}

  G4int PixelYNum( const G4double localYCoord ) const
  {return m_RichGeomProperty->PixelYNumFromCoord(localYCoord) ;}

  G4int GrandPixelXNum(const G4double localXCoord ) const
  {return m_RichGeomProperty->GrandPixelXNumFromCoord(localXCoord) ;}

  G4int GrandPixelYNum ( const G4double localYCoord ) const
  {return m_RichGeomProperty->GrandPixelYNumFromCoord(localYCoord) ;}

protected:

private:
  /// Standard constructor
  CkvCommonSensDet( ); 

private:
  static CkvCommonSensDet* CkvCommonSensDetInstance;
  
  CkvG4GeomProp* m_RichGeomProperty;

  //  RichG4HitCollName* m_RichG4HCName;
  // G4int m_NumberOfHCInRICH;

  // std::vector<CkvG4HitsCollection*>  m_RichHC;
  // std::vector<G4int> m_PhdHCID;
  bool  m_Rich2UseGrandPmt;
  bool m_RichPmtAviodDuplicateHitsActivate;
  bool m_RichPmtFlagDuplicateHitsActivate;
  G4int m_TotNumPmtsInRich;
  std::vector<bool> m_RichPmtAlreadyHit; // flag for pmt to have hits in current event
  std::multimap<G4int,G4int> m_RichPmtToPixelNumMap; // map between pmt and Pixelnum for each hit in current event
  bool m_GeomPropInitFlag;

  CkvG4Hit * m_newHit;
  
};
#endif // GAUSSCHERENKOV_CKVCOMMONSENSDET_H
