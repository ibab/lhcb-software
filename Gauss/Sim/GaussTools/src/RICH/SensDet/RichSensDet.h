//  SE 24-5-2002
// ============================================================================
#ifndef       RichSensDet_H
#define       RichSensDet_H 1 
// ============================================================================
#include "GiGa/GiGaSensDetBase.h"
#include "RichG4Hit.h"
#include "RichG4HitCollName.h"
#include "RichG4GeomProp.h"

/// forward declarations 
template <class SD> 
class GiGaSensDetFactory;
class G4HCofThisEvent;

// class IDataProviderSvc;
//class IMessageSvc;
//class MsgStream;

class RichSensDet: virtual public GiGaSensDetBase
{
  /// friend factory 
  friend class GiGaFactory<RichSensDet>;

protected:
  
  /** standard constructor 
   *  @see GiGaSensDetBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  RichSensDet
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~RichSensDet();
  
public: 
  
  /** process the hit
   *  @param step     pointer to current Geant4 step 
   *  @param history  pointert to touchable history 
   */
  virtual void Initialize(G4HCofThisEvent* HCE);
  virtual void EndOfEvent(G4HCofThisEvent* HCE);
  virtual bool ProcessHits
  ( G4Step* step                , 
    G4TouchableHistory* history ) ;
  virtual void clear();
  virtual void DrawAll();
  virtual void PrintAll();

  RichG4GeomProp*  RichGeomProperty() {return  m_RichGeomProperty; }
  RichG4HitCollName* RichG4HCName() {return m_RichG4HCName; }
  G4int NumberofRichDet() {return  m_RichGeomProperty->NumberOfRichDet(); }

  G4int NumberofHPDsInRich1Det() 
  {return   m_RichGeomProperty->NumberOfHPDsInRich1(); }

  G4int NumberofHPDsInRich2Det() 
  {return m_RichGeomProperty->NumberOfHPDsInRich2(); }

  G4int NumberOfDetSectionsInRich1Det() 
  {return m_RichGeomProperty->NumberOfDetSectionsInRich1() ; }

  G4int NumberOfDetSectionsInRich2Det() 
  {return m_RichGeomProperty->NumberOfDetSectionsInRich2(); }

  G4int NumberofPixelsInHPDofRich() 
  {return m_RichGeomProperty->NumberOfPixelsInHPD(); }

  G4double MaxZHitInRich1Detector() 
  {return m_RichGeomProperty->MaxZHitInRich1Det(); }

  vector<G4double>PixelBoundaryInX() 
  {return m_RichGeomProperty->PixelXBoundary(); }

  vector<G4double>PixelBoundaryInY() 
  {return m_RichGeomProperty->PixelYBoundary(); }

  G4double PixelBoundaryInXValue(int aPXNumber ) 
  {return  m_RichGeomProperty->PixelXBoundaryValue(aPXNumber) ;}
  G4double PixelBoundaryInYValue(int aPYNumber ) 
  {return  m_RichGeomProperty->PixelYBoundaryValue(aPYNumber) ;}

  G4int PixelXNum(G4double localXCoord ) 
  {return m_RichGeomProperty->PixelXNumFromCoord(localXCoord) ;}
  
  G4int PixelYNum(G4double localYCoord ) 
  {return m_RichGeomProperty->PixelYNumFromCoord(localYCoord) ;}

private:
  ///
  RichSensDet(); ///< no default constructor
  RichSensDet( const RichSensDet& ); ///< no copy constructor 
  RichSensDet& operator=( const RichSensDet& ) ; ///< no = 
  ///
private:
  ///
  RichG4GeomProp* m_RichGeomProperty;
  std::vector<G4int> m_Rich1HpdSDID;
  std::vector<G4int> m_Rich2HpdSDID;

  RichG4HitCollName* m_RichG4HCName;
  G4int m_NumberOfHCInRICH;
  std::vector<RichG4HitsCollection*>  m_RichHC;
  std::vector<G4int> m_HpdHCID;
};

// ============================================================================
// The END 
// ============================================================================
#endif 
// ============================================================================
