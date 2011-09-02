#ifndef RichG4GeomProp_h
#define RichG4GeomProp_h 1

#include <vector>
#include "globals.hh"
#include "GiGa/GiGaBase.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

class  RichG4GeomProp  {

public:

  RichG4GeomProp() : m_msgStream(NULL) { }

  RichG4GeomProp(IDataProviderSvc*,IMessageSvc*);

  virtual ~RichG4GeomProp();

  G4int NumberOfRichDet() const {return m_NumberOfRichDet ;}
  G4int NumberOfHPDsInRich1() const {return m_NumberOfHPDsInRich1; }
  G4int NumberOfHPDsInRich2() const {return  m_NumberOfHPDsInRich2; }
  G4int NumberOfDetSectionsInRich1() const {return m_NumberOfDetSectionsInRich1; }
  G4int NumberOfDetSectionsInRich2() const {return m_NumberOfDetSectionsInRich2; }
  G4int NumberOfPixelsInHPD() const {return m_NumberOfPixelsInHPD; }

  G4int NumberOfPixelColInHPD() const { return m_NumberOfPixelColInHPD; }
  G4int NumberOfPixelRowInHPD() const { return m_NumberOfPixelRowInHPD; }
  G4double PixelXsize() const {return m_PixelXsize; }
  G4double PixelYsize() const {return m_PixelYsize; }
  G4double MaxZHitInRich1Det() const {return  m_MaxZHitInRich1Det; }

  const std::vector<G4double> & PixelXBoundary() const {return m_PixelXBoundary; }
  const std::vector<G4double> & PixelYBoundary() const {return m_PixelYBoundary; }

  G4double PixelXBoundaryValue(int PixelXNumber);
  G4double PixelYBoundaryValue(int PixelYNumber);
  G4int PixelXNumFromCoord(const G4double localX );
  G4int PixelYNumFromCoord(const G4double localY );
  G4int PixelPosFinder( G4double Coord,
                        const std::vector<G4double> & BoundaryValues );
  G4int GetPixelNumInHpd(G4int PixelXNum, G4int PixelYNum);
  
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

  std::vector<G4double> m_PixelXBoundary;
  std::vector<G4double> m_PixelYBoundary;
  G4int m_PixelXBoundarySize;
  G4int m_PixelYBoundarySize;

  MsgStream * m_msgStream;

};

inline G4int RichG4GeomProp::PixelXNumFromCoord(const G4double localX)
{
  return PixelPosFinder(localX,m_PixelXBoundary);
}

inline G4int RichG4GeomProp::PixelYNumFromCoord(const G4double localY)
{
  return PixelPosFinder(localY,m_PixelYBoundary);
}

#endif
