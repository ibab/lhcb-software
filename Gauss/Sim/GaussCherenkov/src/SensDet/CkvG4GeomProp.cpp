// $Id: CkvG4GeomProp.cpp,v 1.6 2007-01-12 15:32:19 ranjard Exp $
// Include files 

//local
#include "CkvG4GeomProp.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussCherenkov/CkvG4GaussPathNames.h"
//#include "GaussSuperRich/SRG4GaussPathNames.h"

/// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/DetectorElement.h"

#include "G4ios.hh"
#include <vector>
#include "GaussCherenkov/CkvGeometrySetupUtil.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CkvG4GeomProp
//-----------------------------------------------------------------------------
CkvG4GeomProp::CkvG4GeomProp(IDataProviderSvc* detSvc, IMessageSvc* msgSvc)
{
  //  m_RichGeomPropVerboseLevel=1;
  //  m_NumberOfRichDet=2;
  //  m_NumberOfDetSectionsInRich1=2;
  //  m_NumberOfDetSectionsInRich2=2;
  //  m_NumberofPixelsInHPD=1024;

  // Now to get the total number of HPDs and the Z
  MsgStream log( msgSvc , "GeomProp" );

  DetectorElement*  RichDESD;
  
  CkvGeometrySetupUtil * aCkvGeometrySetup=CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance() ;
  bool isSuperRich = aCkvGeometrySetup->isSuperRich();
  if( isSuperRich) {
    SmartDataPtr<DetectorElement> SuperRichDESD(detSvc,SuperRichDeStructurePathName );
    RichDESD = SuperRichDESD;
  }else {
    SmartDataPtr<DetectorElement> Rich1DESD(detSvc,Rich1DeStructurePathName  );
    RichDESD = Rich1DESD;
    m_NumberOfPMTsInRich1 = RichDESD->param<int>("Rich1TotNumPmt");
    m_NumberOfPMTsInRich2 = RichDESD->param<int>("Rich2TotNumPmt");
    m_NumberOfPMTModulesInRich1= RichDESD->param<int>("Rich1TotNumModules");
    m_NumberOfPMTModulesInRich2= RichDESD->param<int>("Rich2TotNumModules");
    m_NumberOfDetSectionsInRich1= RichDESD->param<int>("Rich1NumberOfDetSections");
    m_NumberOfDetSectionsInRich2= RichDESD->param<int>("Rich2NumberOfDetSections");
    m_MaxZHitInRich1Det = RichDESD->param<double>("Rich1MaxDownstreamZHitCoord");
    
  }
  
  if( !RichDESD )
  {
    log << MSG::ERROR
        << "Can't retrieve "+ Rich1DeStructurePathName+ " or "+  SuperRichDeStructurePathName +  " from RichGeomProp" << endreq;
  }
  else
  {
    // the following lines modified to be compatible with the recent changes in DeDesc.
    // SE 16-6-2005 
    m_NumberOfRichDet= RichDESD->param<int>("RichNumberOfDetectors");
    m_NumberOfPMTsInAModule= RichDESD->param<int>("RichTotNumPmtInModule");
    

    m_NumberOfPixelColInPMT=RichDESD->param<int>("RichPmtNumPixelCol" );
    m_NumberOfPixelRowInPMT=RichDESD->param<int>("RichPmtNumPixelRow" );

    m_PmtPixelXsize=RichDESD->param<double>("RichPmtPixelXsize");
    m_PmtPixelYsize=RichDESD->param<double>("RichPmtPixelYsize");
    m_PmtPixelGap= RichDESD->param<double>("RichPmtPixelGap");

     if( isSuperRich) {
       m_NumberOfPmtsInSuperRich = RichDESD->param<double> ( "SuperRichTotNumPmt" );
       
     }
 
    
   // m_PixelXBoundarySize=m_NumberOfPixelColInHPD+1;
  //  m_PixelYBoundarySize=m_NumberOfPixelRowInHPD+1;
  //  m_PixelXBoundary.reserve( m_PixelXBoundarySize);
  //  m_PixelYBoundary.reserve( m_PixelYBoundarySize);

    m_PmtPixelXBoundarySize=m_NumberOfPixelColInPMT+1;
    m_PmtPixelYBoundarySize=m_NumberOfPixelRowInPMT+1;
    m_PmtPixelXBoundary.reserve( m_PmtPixelXBoundarySize);
    m_PmtPixelYBoundary.reserve( m_PmtPixelYBoundarySize);
    
  //G4double CurPixelBoundaryX= -0.5*m_NumberOfPixelColInHPD*m_PixelXsize;
  //  for(int ipb=0; ipb<m_PixelXBoundarySize; ipb++)
  //  {
  //    m_PixelXBoundary.push_back(CurPixelBoundaryX+ipb*m_PixelXsize);
  //  }
  //  G4double CurPixelBoundaryY= -0.5*m_NumberOfPixelRowInHPD*m_PixelYsize;
  //  for(int ipc=0; ipc<m_PixelYBoundarySize; ipc++) {
  //
  //    m_PixelYBoundary.push_back(CurPixelBoundaryY+ipc*m_PixelYsize);
   // }

    G4double CurPmtPixelBoundaryX=-0.5*RichDESD->param<double>("RichPmtAnodeXSize");
    G4double CurPmtPixelBoundaryY=-0.5*RichDESD->param<double>("RichPmtAnodeYSize");
    
    for(int ipd=0; ipd<m_PmtPixelXBoundarySize; ipd++)
    {
      double aBx= CurPmtPixelBoundaryX+
                    ipd*(m_PmtPixelXsize+m_PmtPixelGap)- 0.5*+m_PmtPixelGap;
      if(ipd == 0 )aBx += 0.5*m_PmtPixelGap;
      if(ipd == (m_PmtPixelXBoundarySize-1)) aBx -= 0.5*m_PmtPixelGap;

      

      m_PmtPixelXBoundary.push_back(aBx);
    }
    for(int ipe=0; ipe<m_PmtPixelYBoundarySize; ipe++)
    {
      double aBy= CurPmtPixelBoundaryY+
                    ipe*(m_PmtPixelYsize+m_PmtPixelGap)- 0.5*+m_PmtPixelGap;
      if(ipe == 0 )aBy += 0.5*m_PmtPixelGap;
      if(ipe == (m_PmtPixelYBoundarySize-1)) aBy -= 0.5*m_PmtPixelGap;

      
      m_PmtPixelYBoundary.push_back(aBy);
    }
    


    // the following line modified to be compatible with recent changes in DetDesc
    // Se 16-6-2005.
    //    m_MaxZHitInRich1Det = Rich1DESD->userParameterAsDouble("Rich1MaxDownstreamZHitCoord");

    //  log << MSG::DEBUG << "Total Number of pmts in Rich1 from RichGeomProp=  "
    //    << m_NumberOfPMTsInRich1
    //    <<"     MaximumZ coord hit in Rich1 fromRichGeomProp = "
    //    << m_MaxZHitInRich1Det<< endreq;
  }

  //  log << MSG::DEBUG<<"Pixel boundary X coord at 0 1 2 16 32 =   "
  //    <<m_PixelXBoundary[0] <<"   "<<m_PixelXBoundary[1]<<"   "
  //    <<m_PixelXBoundary[2] <<"   "<<m_PixelXBoundary[16]
  //    <<"   "<<m_PixelXBoundary[32]<<endreq;
  // log << MSG::DEBUG<<"Pixel boundary Y coord at 0 1 2 16 32 =   "
  //    <<m_PixelYBoundary[0] <<"   "<<m_PixelYBoundary[1]<<"   "
  //    <<m_PixelYBoundary[2] <<"   "<<m_PixelYBoundary[16]
  //    <<"   "<<m_PixelYBoundary[32]<<endreq;

  // log << MSG::VEBOSE<<"Pmt Pixel boundary X coord at 0 1 67 =   "
  //    <<m_PmtPixelXBoundary[0] <<"   "<<m_PmtPixelXBoundary[1]<<"   "
  //    <<m_PmtPixelXBoundary[2] <<"   "<<m_PmtPixelXBoundary[3]<<"   "
  //    <<m_PmtPixelXBoundary[4] <<"   "<<m_PmtPixelXBoundary[5]<<"   "
  //    <<m_PmtPixelXBoundary[6] <<"   "<<m_PmtPixelXBoundary[7]<<endreq;
  // log << MSG::VERBOSE<<"Pmt Pixel boundary Y coord at 0 1 67 =   "
  //    <<m_PmtPixelYBoundary[0] <<"   "<<m_PmtPixelYBoundary[1]<<"   "
  //    <<m_PmtPixelYBoundary[2] <<"   "<<m_PmtPixelYBoundary[3]<<"   "
  //    <<m_PmtPixelYBoundary[4] <<"   "<<m_PmtPixelYBoundary[5]<<"   "
  //    <<m_PmtPixelYBoundary[6] <<"   "<<m_PmtPixelYBoundary[7]<<endreq;


  //  SmartDataPtr<DetectorElement> Rich2DESD(detSvc, Rich2DeStructurePathName);
//  if( !Rich2DESD )
//  {
//    log << MSG::ERROR
//        << "Can't retrieve " +  Rich2DeStructurePathName + " from RichGeomProp" << endreq;
//  }
//  else
//  {  }
}

CkvG4GeomProp::~CkvG4GeomProp() { ; }


G4double CkvG4GeomProp::PixelXBoundaryValue(int PixelXNumber) {

  if(PixelXNumber < (int) m_PmtPixelXBoundary.size()){
    return m_PmtPixelXBoundary[PixelXNumber];
  }
  else
  {
    G4cout<<"RichGeomProp: Unknown Pixel X boundary number in PMT "
          <<PixelXNumber<<G4endl;
    return -1000.0;
  }

}

G4double CkvG4GeomProp::PixelYBoundaryValue(int PixelYNumber) {

  if(PixelYNumber < (int) m_PmtPixelYBoundary.size())
  {
    return m_PmtPixelYBoundary[PixelYNumber];
  }
  else
  {
    G4cout << "RichGeomProp: Unknown Pixel Y boundary number in PMT "
           << PixelYNumber << G4endl;
    return -1000.0;
  }

}
G4bool CkvG4GeomProp::PixelGapFinderX( const G4double Xc ){
  
  G4bool HitInGap=false;
  G4int CurrentPixelNum= PixelXNumFromCoord(Xc);
  
  G4double aDistInPixel= Xc- m_PmtPixelXBoundary[CurrentPixelNum] ;
  if(fabs(aDistInPixel) < (0.5* m_PmtPixelGap) ) HitInGap=true;
  return HitInGap;
  
}

G4bool CkvG4GeomProp::PixelGapFinderY( const G4double Yc ){
  
  G4bool HitInGap=false;
  G4int CurrentPixelNum= PixelYNumFromCoord(Yc);
  
  G4double aDistInPixel= Yc- m_PmtPixelYBoundary[CurrentPixelNum] ;
  if(fabs(aDistInPixel) < (0.5* m_PmtPixelGap) ) HitInGap=true;
  return HitInGap;
  
}


G4int CkvG4GeomProp::PixelPosFinder(G4double Xc,
                                     const std::vector<G4double> & aBound )
{

  G4int CPixel=-1000;

  // G4cout<<" Pixel finder Pmt Pixel boundary "<< aBound[0]<<"  "
  //      <<aBound[1] <<"  "<< aBound[2] << "  "<< aBound[3]<<"  " << aBound[4] 
  //      <<"  "<< aBound[5] <<"  "<< aBound[6] <<"  "
  //      << aBound[7] <<"  "<< aBound[8] <<G4endl;
  
  //  if(Xc < aBound[0] || Xc > aBound[aBound.size()-1] )
  // a tolerence of 0.001 is used to avoid machine precision issues.
  if( (Xc- aBound[0])< (-0.001) || (Xc- aBound[aBound.size()-1])> (0.001) )
  {
    G4cout << " RichGeomProp: Pixel coord outside PmtAnode "
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

G4int CkvG4GeomProp::GetPixelNumInPmt( int PixelXNum, int PixelYNum) 
{
  return (PixelXNum + (m_NumberOfPixelColInPMT * PixelYNum));
  
  
}





