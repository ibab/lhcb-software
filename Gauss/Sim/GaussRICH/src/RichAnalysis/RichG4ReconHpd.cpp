// $Id: RichG4ReconHpd.cpp,v 1.1 2004-02-04 13:53:01 seaso Exp $
// Include files 

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/TabulatedProperty.h"

// local
#include "RichG4SvcLocator.h"

// local
#include "RichG4ReconHpd.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4ReconHpd
//
// 2003-09-09 : Sajan EASO
//-----------------------------------------------------------------------------
//
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4ReconHpd::RichG4ReconHpd():
    m_NumHpdInternalDimensions(10),
    m_HpdInternalDimensions
    (std::vector<double>(m_NumHpdInternalDimensions)),
    m_NumHpdCrossFocusParameters(2),
    m_HpdCrossFocusParameters
     (std::vector<double>( m_NumHpdCrossFocusParameters))
{
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  
  MsgStream RichG4HpdReconlog( msgSvc,"RichG4HpdRecon");

    SmartDataPtr<IDetectorElement> Rich1DE(detSvc, "/dd/Structure/LHCb/Rich1");

  if( !Rich1DE ){
    RichG4HpdReconlog << MSG::ERROR 
               << "Can't retrieve /dd/Structure/LHCb/Rich1 for HpdRecon" 
               << endreq;
  }else {

    m_HpdSiDetXSize=
      Rich1DE->userParameterAsDouble("RichHpdSiliconDetectorXSize");
    
    m_HpdSiDetYSize=
      Rich1DE->userParameterAsDouble("RichHpdSiliconDetectorYSize");
    
    m_HpdSiDetZSize=
      Rich1DE->userParameterAsDouble("RichHpdSiliconDetectorZSize");
    
    m_HpdSiDetZLocation= 
      Rich1DE->userParameterAsDouble("RichHpdSiliconDetectorLocalZlocation");
    
    m_HpdPhCathodeToSiDetMaxDist=
      Rich1DE->userParameterAsDouble("RichHpdQWToSiMaxDist");
    
    //    m_HpdPhCathodeRad=
    //  Rich1DE->userParameterAsDouble("RichHpdQWInnerSphericalRadius");
    m_HpdPhCathodeRad=
      Rich1DE->userParameterAsDouble("RichHpdPhCathInnerSphericalRadius");

    m_HpdPixelXSize=
      Rich1DE->userParameterAsDouble( "RichHpdPixelXsize");
    
    m_HpdPixelYSize=  
      Rich1DE->userParameterAsDouble("RichHpdPixelYsize");

    m_HpdPixelXNumCol=
      Rich1DE->userParameterAsDouble( "RichHpdNumPixelCol");

    m_HpdPixelXNumRow=
      Rich1DE->userParameterAsDouble("RichHpdNumPixelRow" );

    double PhCathiodeCoCZ= 
      Rich1DE->userParameterAsDouble("RichHpdPhCathodeCoCZ");
    
    //        RichG4HpdReconlog<<MSG::INFO
    //  << "PhCathodetoSiDist SiDetZ Phcathoderad phcathZ "
    //                 <<m_HpdPhCathodeToSiDetMaxDist
    //                 <<"  "<<m_HpdSiDetZLocation
    //                 <<"  "<< m_HpdPhCathodeRad<< "  "
    //                     <<PhCathiodeCoCZ<< endreq;
    
  }
  

   SmartDataPtr<TabulatedProperty>tabDemag(detSvc,
     "/dd/Materials/RichMaterialTabProperties/HpdDemagnification");
  std::vector<double>HpdDemagFac; 
  if(!tabDemag) {
   RichG4HpdReconlog << MSG::ERROR <<"RichG4CkvRecon: Can't retrieve  "
     <<" /dd/Materials/RichMaterialTabProperties/HpdDemagnification "
               <<endreq;

  }else{

    TabulatedProperty::Table tableDem = tabDemag->table();
    TabulatedProperty::Table::iterator itd;

    for (itd = tableDem.begin(); itd != tableDem.end(); itd++) {
                 
      HpdDemagFac.push_back(itd->second);
    }
  }
  
  if( HpdDemagFac.size() >=1  ) {  
    // Now for the cross-focussing effect.  
    if(HpdDemagFac[0] != 0.0 )HpdDemagFac[0] = -1.0*HpdDemagFac[0];
    m_HpdCrossFocusParameters[0]=HpdDemagFac[0];   
  }
  
    if( HpdDemagFac.size() >=2 ) {
      
      m_HpdCrossFocusParameters[1]=HpdDemagFac[1];

       
    }
    
  

};

RichG4ReconHpd::~RichG4ReconHpd() 
{
};

HepPoint3D RichG4ReconHpd::ConvertLocalHitToHpdSystem(HepPoint3D aLocalHit) 
{
  double zloc = aLocalHit.z();
  double ZInHpdSystem = zloc - m_HpdSiDetZLocation;
  return HepPoint3D(aLocalHit.x(),aLocalHit.y(), ZInHpdSystem);
  
}
double RichG4ReconHpd::ConvertHpdSiliconZToHpdSystem(double aZcoord ) 
{
 return   (aZcoord + m_HpdSiDetZLocation); 
};


HepPoint3D RichG4ReconHpd::ReconHitOnPhCathFromLocalHitCoord 
 (HepPoint3D aLocalHitCoord){

  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  
  MsgStream RichG4HpdReconlog( msgSvc,"RichG4HpdRecon");

  double xsi = aLocalHitCoord.x();
  
  double ysi = aLocalHitCoord.y();
  
  double zsi = aLocalHitCoord.z();
  double xph =0.0;
  double yph =0.0;
  double zph =0.0;
  double rphsq=0.0;
  double phCSq=0.0;
  double delZ=0.0;
  double zPhInHpd=0.0;
  
  if(m_HpdCrossFocusParameters[0] !=0.0 ) {
    xph= xsi/m_HpdCrossFocusParameters[0];
    yph= ysi/m_HpdCrossFocusParameters[0];
  }

  rphsq = xph*xph + yph*yph;
  phCSq = m_HpdPhCathodeRad*m_HpdPhCathodeRad;
  if( ( phCSq -  rphsq) > 0.0  ) {
     
    delZ = pow( ( phCSq -  rphsq ),0.5);
    
  } else {
    delZ= m_HpdPhCathodeRad;
    
  }
  
  zph= m_HpdPhCathodeToSiDetMaxDist-m_HpdPhCathodeRad+ delZ;

  //  RichG4HpdReconlog<<MSG::VERBOSE
  //        <<" ReconHitOnPhCathFromLocalHitCoord "
  //         <<"  xph yph zph phcathrad delZ "
  //         <<xph<<"  "<<yph<<"  "
  //         <<zph<<"  "<<m_HpdPhCathodeRad<<"  "
  //         <<delZ<<endreq;
  
  zPhInHpd =   ConvertHpdSiliconZToHpdSystem(zph) ;
    return HepPoint3D(xph,yph, zPhInHpd);
};



//=============================================================================
