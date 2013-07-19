#ifndef CkvG4GeomProp_h
#define CkvG4GeomProp_h 1
#include <vector>
#include "globals.hh"
#include "GiGa/GiGaBase.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

class  CkvG4GeomProp  {

public:

  CkvG4GeomProp(IDataProviderSvc*,IMessageSvc*);
  virtual ~CkvG4GeomProp();


  G4int NumberOfRichDet() const {return m_NumberOfRichDet ;}
  G4int NumberOfPMTsInRich1() const {return m_NumberOfPMTsInRich1; }
  G4int NumberOfPMTsInRich2() const {return  m_NumberOfPMTsInRich2; }

  G4int NumberOfPMTModulesInRich1()  {  return  m_NumberOfPMTModulesInRich1;}
  G4int NumberOfPMTModulesInRich2()  {  return  m_NumberOfPMTModulesInRich2;}
  G4int NumberOfPMTsInAModule()        {  return m_NumberOfPMTsInAModule;}
  

  G4int NumberOfDetSectionsInRich1() const {return m_NumberOfDetSectionsInRich1; }
  G4int NumberOfDetSectionsInRich2() const {return m_NumberOfDetSectionsInRich2; }
  G4int NumberOfPixelsInPMT() const {return m_NumberOfPixelsInPMT; }

  G4int NumberOfPixelColInPMT() const { return m_NumberOfPixelColInPMT; }
  G4int NumberOfPixelRowInPMT() const { return m_NumberOfPixelRowInPMT; }
  G4double PixelXsize() const {return m_PmtPixelXsize; }
  G4double PixelYsize() const {return m_PmtPixelYsize; }
  G4double MaxZHitInRich1Det() const {return  m_MaxZHitInRich1Det; }

  const std::vector<G4double> & PixelXBoundary() const {return m_PmtPixelXBoundary; }
  const std::vector<G4double> & PixelYBoundary() const {return m_PmtPixelYBoundary; }

  G4double PixelXBoundaryValue(int PixelXNumber);
  G4double PixelYBoundaryValue(int PixelYNumber);
  G4int PixelXNumFromCoord(const G4double localX );
  G4int PixelYNumFromCoord(const G4double localY );
  G4int PixelPosFinder( G4double Coord,
                        const std::vector<G4double> & BoundaryValues );


  G4bool PixelGapFinderX( const G4double Xc );
  G4bool PixelGapFinderY( const G4double Yc );
  G4int NumberOfPmtsInSuperRich()  {  return    m_NumberOfPmtsInSuperRich;}
  G4int GetPixelNumInPmt( int PixelXNum, int PixelYNum);
  
  
private:

  G4int m_NumberOfRichDet;
  //  G4int m_NumberOfHPDsInRich1;
  // G4int m_NumberOfHPDsInRich2;
  G4int m_NumberOfDetSectionsInRich1;
  G4int m_NumberOfDetSectionsInRich2;
  //  G4int m_NumberOfPixelsInHPD;
  G4int m_NumberOfPixelsInPMT;

  //  G4int m_NumberOfPixelColInHPD;
  // G4int m_NumberOfPixelRowInHPD;
  G4int m_NumberOfPixelColInPMT;
  G4int m_NumberOfPixelRowInPMT;


  G4double m_PixelXsize;
  G4double m_PixelYsize;

  G4int m_NumberOfPMTsInRich1;
  G4int m_NumberOfPMTsInRich2;

  G4int m_NumberOfPMTModulesInRich1;
  G4int m_NumberOfPMTModulesInRich2;
  G4int m_NumberOfPMTsInAModule;

  G4double m_PmtPixelXsize;
  G4double m_PmtPixelYsize;
  G4double m_PmtPixelGap;
  
  G4double  m_MaxZHitInRich1Det;

  std::vector<G4double> m_PixelXBoundary;
  std::vector<G4double> m_PixelYBoundary;
  G4int m_PixelXBoundarySize;
  G4int m_PixelYBoundarySize;
  std::vector<G4double> m_PmtPixelXBoundary;
  std::vector<G4double> m_PmtPixelYBoundary;
  G4int m_PmtPixelXBoundarySize;
  G4int m_PmtPixelYBoundarySize;

  G4int m_NumberOfPmtsInSuperRich;
  

};

inline G4int CkvG4GeomProp::PixelXNumFromCoord(const G4double localX)
{
  return PixelPosFinder(localX,m_PmtPixelXBoundary);
}

inline G4int CkvG4GeomProp::PixelYNumFromCoord(const G4double localY)
{
  return PixelPosFinder(localY,m_PmtPixelYBoundary);
}

#endif
