// $Id: CkvSensDet.h,v 1.6 2009-07-17 13:46:13 jonrob Exp $
#ifndef       CkvSensDet_H
#define       CkvSensDet_H 1

// Include files
// from GiGa
#include "GiGa/GiGaSensDetBase.h"

// local
#include "GaussCherenkov/CkvG4Hit.h"
#include "GaussRICH/RichG4HitCollName.h"
#include "CkvG4GeomProp.h"
#include <map>

// forward declarations
class G4HCofThisEvent;

/** @class CkvSensDet CkvSensDet.h src/SensDet/CkvSensDet.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2002-05-24, last modified 2007-01-11
 */

class CkvSensDet: virtual public GiGaSensDetBase
{

public:

  /** standard constructor
   *  @see GiGaSensDetBase
   *  @see GiGaBase
   *  @see AlgTool
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  CkvSensDet
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;

  /// destructor (virtual and protected)
  virtual ~CkvSensDet();
  /// initialize
  StatusCode initialize();

  /// finalize
  StatusCode finalize();

  /** process the hit
   *  @param step     pointer to current Geant4 step
   *  @param history  pointert to touchable history
   */
  virtual void Initialize(G4HCofThisEvent* HCE);
  // virtual void EndOfEvent(G4HCofThisEvent* HCE);
  virtual bool ProcessHits
  ( G4Step* step                ,
    G4TouchableHistory* history ) ;
  virtual void clear();
  virtual void DrawAll();
  virtual void PrintAll();

  CkvG4GeomProp*  RichGeomProperty() {return  m_RichGeomProperty; }
  RichG4HitCollName* RichG4HCName() {return m_RichG4HCName; }
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

  void ResetPmtMapInCurrentEvent();
 
private:
  ///
  CkvSensDet(); ///< no default constructor
  CkvSensDet( const CkvSensDet& ); ///< no copy constructor
  CkvSensDet& operator=( const CkvSensDet& ) ; ///< no =
  ///
private:
  ///
  CkvG4GeomProp* m_RichGeomProperty;
  std::vector<G4int> m_Rich1PhdSDID;
  std::vector<G4int> m_Rich2PhdSDID;

  RichG4HitCollName* m_RichG4HCName;
  G4int m_NumberOfHCInRICH;
  std::vector<CkvG4HitsCollection*>  m_RichHC;
  std::vector<G4int> m_PhdHCID;
  bool m_SuperRichFlag;
  bool  m_OptHorizRichFlag;

  bool m_RichPmtAviodDuplicateHitsActivate;
  bool m_RichPmtFlagDuplicateHitsActivate;
  G4int m_TotNumPmtsInRich;
  std::vector<bool> m_RichPmtAlreadyHit; // flag for pmt to have hits in current event
  std::multimap<G4int,G4int> m_RichPmtToPixelNumMap; // map between pmt and Pixelnum for each hit in current event
  

};

#endif
