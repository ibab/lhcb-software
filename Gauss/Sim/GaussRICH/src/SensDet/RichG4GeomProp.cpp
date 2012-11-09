// $Id: RichG4GeomProp.cpp,v 1.6 2007-01-12 15:32:19 ranjard Exp $
// Include files 

//local
#include "RichG4GeomProp.h"
#include "GaussRICH/RichG4GaussPathNames.h"

/// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/DetectorElement.h"

#include "G4ios.hh"
#include <vector>

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4GeomProp
//-----------------------------------------------------------------------------
RichG4GeomProp::RichG4GeomProp(IDataProviderSvc* detSvc, IMessageSvc* msgSvc)
{
  //  m_RichGeomPropVerboseLevel=1;
  //  m_NumberOfRichDet=2;
  //  m_NumberOfDetSectionsInRich1=2;
  //  m_NumberOfDetSectionsInRich2=2;
  //  m_NumberofPixelsInHPD=1024;

  m_msgStream = new MsgStream( msgSvc, "RichG4GeomProp" );
  MsgStream & log = *m_msgStream;

  // Now to get the total number of HPDs and the Z
  SmartDataPtr<DetectorElement> Rich1DESD(detSvc,Rich1DeStructurePathName  );

  if( !Rich1DESD )
  {
    log << MSG::ERROR
        << "Can't retrieve "+ Rich1DeStructurePathName+ " from RichGeomProp" << endreq;
  }
  else
  {
    // the following lines modified to be compatible with the recent changes in DeDesc.
    // SE 16-6-2005 
    m_NumberOfRichDet= Rich1DESD->param<int>("RichNumberOfDetectors");
    m_NumberOfDetSectionsInRich1= Rich1DESD->param<int>("Rich1NumberOfDetSections");
    m_NumberOfDetSectionsInRich2= Rich1DESD->param<int>("Rich2NumberOfDetSections");
    m_NumberOfPixelsInHPD=Rich1DESD->param<int>("RichHpdTotNumPixel");

    m_NumberOfHPDsInRich1 = Rich1DESD->param<int>("Rich1TotNumHpd");
    m_NumberOfHPDsInRich2 = Rich1DESD->param<int>("Rich2TotNumHpd");
    m_NumberOfPixelColInHPD=Rich1DESD->param<int>("RichHpdNumPixelCol" );
    m_NumberOfPixelRowInHPD=Rich1DESD->param<int>("RichHpdNumPixelRow" );
    m_PixelXsize=Rich1DESD->param<double>("RichHpdPixelXsize");
    m_PixelYsize=Rich1DESD->param<double>("RichHpdPixelYsize");

    //    m_NumberOfRichDet= Rich1DESD->userParameterAsInt("RichNumberOfDetectors");
    // m_NumberOfDetSectionsInRich1= Rich1DESD->userParameterAsInt("Rich1NumberOfDetSections");
    // m_NumberOfDetSectionsInRich2= Rich1DESD->userParameterAsInt("Rich2NumberOfDetSections");
    //  m_NumberOfPixelsInHPD=Rich1DESD->userParameterAsInt("RichHpdTotNumPixel");

    // m_NumberOfHPDsInRich1 = Rich1DESD->userParameterAsInt("Rich1TotNumHpd");
    // m_NumberOfHPDsInRich2 = Rich1DESD->userParameterAsInt("Rich2TotNumHpd");
    // m_NumberOfPixelColInHPD=Rich1DESD->userParameterAsInt("RichHpdNumPixelCol" );
    // m_NumberOfPixelRowInHPD=Rich1DESD->userParameterAsInt("RichHpdNumPixelRow" );
    // m_PixelXsize=Rich1DESD->userParameterAsDouble("RichHpdPixelXsize");
    // m_PixelYsize=Rich1DESD->userParameterAsDouble("RichHpdPixelYsize");

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

    // the following line modified to be compatible with recent changes in DetDesc
    // Se 16-6-2005.
    m_MaxZHitInRich1Det = Rich1DESD->param<double>("Rich1MaxDownstreamZHitCoord");
    //    m_MaxZHitInRich1Det = Rich1DESD->userParameterAsDouble("Rich1MaxDownstreamZHitCoord");

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

  SmartDataPtr<DetectorElement> Rich2DESD(detSvc, Rich2DeStructurePathName);
  if( !Rich2DESD )
  {
    log << MSG::ERROR
        << "Can't retrieve " +  Rich2DeStructurePathName + " from RichGeomProp" << endreq;
  }
  else
  {
    //the following line already made earlier and hence is unnecessary.
    // eventually this line should be activated so that the parameter is
    // read from the rich2 sub directory in xml.    

    //m_NumberofHPDsInRich2 = Rich2DESD->param<int>("Rich2TotNumHpd");


    //m_NumberofHPDsInRich2 = Rich2DESD->userParameterAsInt("Rich2TotNumHpd");
  }
}

RichG4GeomProp::~RichG4GeomProp() 
{
  delete m_msgStream;
}

G4double RichG4GeomProp::PixelXBoundaryValue(int PixelXNumber) 
{
  if(PixelXNumber < (int) m_PixelXBoundary.size())
  {
    return m_PixelXBoundary[PixelXNumber];
  }
  else
  {
    if ( m_msgStream )
      *m_msgStream << MSG::WARNING 
                   << "Unknown Pixel X boundary number in HPD "
                   << PixelXNumber << endmsg;
    return -1000.0;
  }
}

G4double RichG4GeomProp::PixelYBoundaryValue(int PixelYNumber) 
{
  if(PixelYNumber < (int) m_PixelYBoundary.size())
  {
    return m_PixelYBoundary[PixelYNumber];
  }
  else
  {
    if ( m_msgStream )
      *m_msgStream << MSG::WARNING 
                   << "Unknown Pixel Y boundary number in HPD "
                   << PixelYNumber << endmsg;
    return -1000.0;
  }
}

G4int RichG4GeomProp::PixelPosFinder(G4double Xc,
                                     const std::vector<G4double> & aBound )
{

  G4int CPixel=-1000;

  if(Xc < aBound[0] || Xc > aBound[aBound.size()-1] )
  {
    if ( m_msgStream )
      *m_msgStream << MSG::WARNING 
                   << "Pixel coord outside Sidet. Coord and Pixel bounds = "
                   <<Xc<<"    "<< aBound[0]<<"    "<< aBound[aBound.size()-1]
                   <<endmsg;
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

G4int RichG4GeomProp::GetPixelNumInHpd(G4int PixelXNum, G4int PixelYNum) 
{
  return (PixelXNum + (m_NumberOfPixelColInHPD * PixelYNum)) ;
  
}

