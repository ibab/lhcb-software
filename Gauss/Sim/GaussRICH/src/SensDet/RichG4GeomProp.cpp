//local
#include "RichG4GeomProp.h"

/// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/DetectorElement.h"

/// GiGa
#include "GiGa/GiGaMACROs.h"

#include "G4ios.hh"
#include <vector>

RichG4GeomProp::RichG4GeomProp(IDataProviderSvc* detSvc, IMessageSvc* msgSvc)
{
  //  m_RichGeomPropVerboseLevel=1;
  //  m_NumberOfRichDet=2;
  //  m_NumberOfDetSectionsInRich1=2;
  //  m_NumberOfDetSectionsInRich2=2;
  //  m_NumberofPixelsInHPD=1024;

  // Now to get the total number of HPDs and the Z
  MsgStream log( msgSvc , "GeomProp" );
  SmartDataPtr<DetectorElement> Rich1DESD(detSvc, "/dd/Structure/LHCb/Rich1");

  if( !Rich1DESD )
  {
    log << MSG::ERROR
        << "Can't retrieve /dd/Structure/LHCb/Rich1 from RichGeomProp" << endreq;
  }
  else
  {
    m_NumberOfRichDet= Rich1DESD->userParameterAsInt("RichNumberOfDetectors");
    m_NumberOfDetSectionsInRich1= Rich1DESD->userParameterAsInt("Rich1NumberOfDetSections");
    m_NumberOfDetSectionsInRich2= Rich1DESD->userParameterAsInt("Rich2NumberOfDetSections");
    m_NumberOfPixelsInHPD=Rich1DESD->userParameterAsInt("RichHpdTotNumPixel");

    m_NumberOfHPDsInRich1 = Rich1DESD->userParameterAsInt("Rich1TotNumHpd");
    m_NumberOfHPDsInRich2 = Rich1DESD->userParameterAsInt("Rich2TotNumHpd");
    m_NumberOfPixelColInHPD=Rich1DESD->userParameterAsInt("RichHpdNumPixelCol" );
    m_NumberOfPixelRowInHPD=Rich1DESD->userParameterAsInt("RichHpdNumPixelRow" );
    m_PixelXsize=Rich1DESD->userParameterAsDouble("RichHpdPixelXsize");
    m_PixelYsize=Rich1DESD->userParameterAsDouble("RichHpdPixelYsize");
    m_PixelXBoundarySize=m_NumberOfPixelColInHPD+1;
    m_PixelYBoundarySize=m_NumberOfPixelRowInHPD+1;
    m_PixelXBoundary.reserve( m_PixelXBoundarySize);
    m_PixelYBoundary.reserve( m_PixelYBoundarySize);
    G4double CurPixelBoundaryX= -0.5*m_NumberOfPixelColInHPD*m_PixelXsize;
    for(int ipb=0; ipb<m_PixelXBoundarySize; ipb++)
    {
      m_PixelXBoundary.push_back(CurPixelBoundaryX+ipb*m_PixelXsize);
    }
    G4double CurPixelBoundaryY= -0.5*m_NumberOfPixelRowInHPD*m_PixelYsize;
    for(int ipc=0; ipc<m_PixelYBoundarySize; ipc++) {

      m_PixelYBoundary.push_back(CurPixelBoundaryY+ipc*m_PixelYsize);
    }

    m_MaxZHitInRich1Det = Rich1DESD->userParameterAsDouble("Rich1MaxDownstreamZHitCoord");

    log << MSG::DEBUG << "Total Number of hpds in Rich1 from RichGeomProp=  "
        << m_NumberOfHPDsInRich1
        <<"     MaximumZ coord hit in Rich1 fromRichGeomProp = "
        << m_MaxZHitInRich1Det<< endreq;
  }

  log << MSG::DEBUG<<"Pixel boundary X coord at 0 1 2 16 32 =   "
      <<m_PixelXBoundary[0] <<"   "<<m_PixelXBoundary[1]<<"   "
      <<m_PixelXBoundary[2] <<"   "<<m_PixelXBoundary[16]
      <<"   "<<m_PixelXBoundary[32]<<endreq;
  log << MSG::DEBUG<<"Pixel boundary Y coord at 0 1 2 16 32 =   "
      <<m_PixelYBoundary[0] <<"   "<<m_PixelYBoundary[1]<<"   "
      <<m_PixelYBoundary[2] <<"   "<<m_PixelYBoundary[16]
      <<"   "<<m_PixelYBoundary[32]<<endreq;

  SmartDataPtr<DetectorElement> Rich2DESD(detSvc, "/dd/Structure/LHCb/Rich2");
  if( !Rich2DESD )
  {
    log << MSG::ERROR
        << "Can't retrieve /dd/Structure/LHCb/Rich2 from RichGeomProp" << endreq;
  }
  else
  {
    //the following line already made earlier and hence is unnecessary.

    //m_NumberofHPDsInRich2 = Rich2DESD->userParameterAsInt("Rich2TotNumHpd");
  }
}

RichG4GeomProp::~RichG4GeomProp() { ; }


G4double RichG4GeomProp::PixelXBoundaryValue(int PixelXNumber) {

  if(PixelXNumber < (int) m_PixelXBoundary.size()){
    return m_PixelXBoundary[PixelXNumber];
  }
  else
  {
    G4cout<<"RichGeomProp: Unknown Pixel X boundary number in HPD "
          <<PixelXNumber<<G4endl;
    return -1000.0;
  }

}

G4double RichG4GeomProp::PixelYBoundaryValue(int PixelYNumber) {

  if(PixelYNumber < (int) m_PixelYBoundary.size())
  {
    return m_PixelYBoundary[PixelYNumber];
  }
  else
  {
    G4cout << "RichGeomProp: Unknown Pixel Y boundary number in HPD "
           << PixelYNumber << G4endl;
    return -1000.0;
  }

}

G4int RichG4GeomProp::PixelPosFinder(G4double Xc,
                                     const std::vector<G4double> & aBound )
{

  G4int CPixel=-1000;

  if(Xc < aBound[0] || Xc > aBound[aBound.size()-1] )
  {
    G4cout << " RichGeomProp: Pixel coord outside Sidet "
           <<" Coord and Pixel bounds =  "
           <<Xc<<"    "<< aBound[0]<<"    "<< aBound[aBound.size()-1]
           <<G4endl;
  }
  else
  {
    //Now do a binary search.
    G4int mid;
    G4int left=0;
    G4int right=aBound.size()-1;
    do
    {
      mid=(left+right)/2;
      if(aBound[mid] < Xc ) {
        left = mid;
      }
      else
      {
        right=mid;
      }

    }
    while ( (right-left) > 1 );

    CPixel=left;
  }
  return CPixel;
}

