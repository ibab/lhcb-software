#ifndef RichG4GeomProp_h
#define RichG4GeomProp_h 1
#include <vector>
#include "globals.hh"
#include "GiGa/GiGaBase.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

class  RichG4GeomProp  {

public:

  RichG4GeomProp(IDataProviderSvc*,IMessageSvc*);
  virtual ~RichG4GeomProp();
   

  G4int NumberOfRichDet() {return  m_NumberOfRichDet ;}
  G4int NumberOfHPDsInRich1() {return m_NumberOfHPDsInRich1; }
  G4int NumberOfHPDsInRich2() {return  m_NumberOfHPDsInRich2; }
  G4int NumberOfDetSectionsInRich1() {return m_NumberOfDetSectionsInRich1; }
  G4int NumberOfDetSectionsInRich2() {return m_NumberOfDetSectionsInRich2; }
  G4int NumberOfPixelsInHPD() {return m_NumberOfPixelsInHPD; }

  G4int NumberOfPixelColInHPD() { return m_NumberOfPixelColInHPD; }
  G4int NumberOfPixelRowInHPD() { return m_NumberOfPixelRowInHPD; }
  G4double PixelXsize() {return m_PixelXsize; }
  G4double PixelYsize() {return m_PixelYsize; }
  G4double MaxZHitInRich1Det() {return  m_MaxZHitInRich1Det; }

  std::vector<G4double>PixelXBoundary() {return m_PixelXBoundary; }
  std::vector<G4double>PixelYBoundary() {return m_PixelYBoundary; }
  G4double PixelXBoundaryValue(int PixelXNumber); 
  G4double PixelYBoundaryValue(int PixelYNumber); 
  G4int PixelXNumFromCoord(G4double localX );
  G4int PixelYNumFromCoord(G4double localY );
  G4int PixelPosFinder(G4double Coord, std::vector<G4double>BoundaryValues );

private:

  G4int m_NumberOfRichDet;
  G4int m_NumberOfHPDsInRich1;
  G4int m_NumberOfHPDsInRich2;
  G4int m_NumberOfDetSectionsInRich1;
  G4int m_NumberOfDetSectionsInRich2;
  G4int m_NumberOfPixelsInHPD;

  G4int m_NumberOfPixelColInHPD;
  G4int m_NumberOfPixelRowInHPD;
  G4double m_PixelXsize;
  G4double m_PixelYsize;

  G4double  m_MaxZHitInRich1Det;

  std::vector<G4double>m_PixelXBoundary;
  std::vector<G4double>m_PixelYBoundary;
  G4int m_PixelXBoundarySize;
  G4int m_PixelYBoundarySize;


};

#endif
