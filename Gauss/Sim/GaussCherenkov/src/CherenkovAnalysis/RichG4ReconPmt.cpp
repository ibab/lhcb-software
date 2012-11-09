// $Id: RichG4ReconHpd.cpp,v 1.12 2009-07-03 11:59:49 seaso Exp $
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
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// local
#include "GaussCherenkov/CkvG4SvcLocator.h"

// local
#include "RichG4ReconPmt.h"

// RichDet
#include "RichDet/DeRich.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4ReconHpd
//
// 2003-09-09 : Sajan EASO
//-----------------------------------------------------------------------------
//
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4ReconPmt::RichG4ReconPmt(){
  //  m_NumPmtInternalDimensions(10),
  // m_PmtInternalDimensions(std::vector<double>(m_NumPmtInternalDimensions))


  IDataProviderSvc* detSvc = CkvG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();

  MsgStream RichG4PmtReconlog( msgSvc,"RichG4PmtRecon");

    SmartDataPtr<DetectorElement> Rich1DE(detSvc, DeRichLocations::Rich1);

  if( !Rich1DE ){
    RichG4PmtReconlog << MSG::ERROR
                      << "Can't retrieve " << DeRichLocations::Rich1 << " for PmtRecon"
                      << endreq;
  }else {

    m_PmtAnodeXSize=
      Rich1DE->param<double>("RichPmtAnodeXSize");
    m_PmtAnodeYSize=
      Rich1DE->param<double>("RichPmtAnodeYSize");
    m_PmtAnodeZSize=
      Rich1DE->param<double>("RichPmtAnodeZSize");



    m_PmtAnodeZLocation=
      Rich1DE->param<double>("RichPmtSiliconDetectorLocalZlocation");



    m_PmtPhCathodeToAnodeMaxDist=
      Rich1DE->param<double>("RichPmtQWToSiMaxDist");


    m_PmtPixelXSize=
      Rich1DE->param<double>( "RichPmtPixelXsize");


    m_PmtPixelYSize=
      Rich1DE->param<double>("RichPmtPixelYsize");


    m_PmtPixelXNumCol=
      Rich1DE->param<int>( "RichPmtNumPixelCol");


    m_PmtPixelXNumRow=
      Rich1DE->param<int>("RichPmtNumPixelRow" );

    m_PmtAnodeSurfaceZLocation=
      Rich1DE->param<double> ( "RichPmtSiliconDetectorSurfaceLocalZlocation" );
    m_PmtPhCathZLocation=
      Rich1DE->param<double> ("RichPmtPhCathZLocation" );
    m_PmtQwSurfaceZLocation =
      Rich1DE->param<double> ("RichPmtQWExternalZPos" );
    

    //   RichG4PmtReconlog << MSG::INFO<<" Pmt local Z location of Si sisurface phcath Qw external "  
    //                  <<m_PmtAnodeZLocation<<"  "<<m_PmtAnodeSurfaceZLocation<<"   "
    //                  <<m_PmtPhCathZLocation <<"   "<<m_PmtQwSurfaceZLocation<<endreq;
    

  }







};

RichG4ReconPmt::~RichG4ReconPmt()
{
};

Gaudi::XYZPoint RichG4ReconPmt::ConvertLocalHitToPmtSystem(const Gaudi::XYZPoint & aLocalHit)
{
  double zloc = aLocalHit.z();
  double ZInPmtSystem = zloc - m_PmtAnodeZLocation;
  return Gaudi::XYZPoint(aLocalHit.x(),aLocalHit.y(), ZInPmtSystem);
}

Gaudi::XYZPoint
RichG4ReconPmt::ReconHitOnPhCathFromLocalHitCoord ( const Gaudi::XYZPoint & aLocalHitCoord)
{

  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();

  MsgStream RichG4PmtReconlog( msgSvc,"RichG4PmtRecon");

  double xsi = aLocalHitCoord.x();

  double ysi = aLocalHitCoord.y();

  double xph = xsi;
  double yph = ysi;
  double zph = m_PmtPhCathodeToAnodeMaxDist;
  double ZPhInPmt = ConvertPmtAnodeZToPmtSystem(zph);
    return Gaudi::XYZPoint(xph,yph, ZPhInPmt);
};



//=============================================================================
