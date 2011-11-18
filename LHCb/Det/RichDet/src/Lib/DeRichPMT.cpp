// $Id: $
// Include files 

#include <sstream>

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

// DetDesc
#include "DetDesc/Condition.h"
#include "DetDesc/SolidBox.h"

// local
#include "RichDet/DeRichPMT.h"
#include "RichDet/DeRich.h"
#include "RichDet/DeRichSystem.h"


//-----------------------------------------------------------------------------
// Implementation file for class : DeRichPMT
//
// 2011-10-11 : Sajan Easo
//-----------------------------------------------------------------------------
const CLID CLID_DeRichPMT = 12025;  // User defined

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichPMT::DeRichPMT( const std::string & name ): 
  DeRichPD               ( name  )
 {

 }
//=============================================================================
// Destructor
//=============================================================================
DeRichPMT::~DeRichPMT() {} 

//=============================================================================
const CLID& DeRichPMT::classID()
{
  return CLID_DeRichPMT;
}
StatusCode DeRichPMT::initialize ( )
{
  StatusCode  sc = StatusCode::SUCCESS;
  MsgStream msg( msgSvc(), "DeRichPMT" );

  // store the name of the PMT, without the /dd/Structure part
  const std::string::size_type pos = name().find("MAPMT:");
  setMyName( std::string::npos != pos ? name().substr(pos) : "DeRichPMT_NO_NAME" );
  if ( msgLevel(MSG::DEBUG,msg) )
      msg << MSG::DEBUG << "Initialize " << myName() << endmsg;

 // extract PMT overall copy number from detector element name
  const std::string::size_type pos2 = name().find(":");
  if ( std::string::npos == pos2 )
  {
    msg << MSG::FATAL << "A PMT without a number!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_number = atoi( name().substr(pos2+1).c_str() );
  

  m_dePmtAnode = ( !childIDetectorElements().empty() ?
                   childIDetectorElements().front() : NULL );
  if ( !m_dePmtAnode )
  {
    fatal() << "DePMT : Cannot find PmtAnode detector element" << endmsg;
    return StatusCode::FAILURE;
  }

  sc = sc &&  getPMTParameters();
  
  updMgrSvc()->registerCondition( this, geometry(),
                                  &DeRichPMT::updateGeometry );
  updMgrSvc()->registerCondition( this, m_dePmtAnode->geometry(),
                                  &DeRichPMT::updateGeometry );


  // Update PMT QE values whenever DeRichSystem updates
  updMgrSvc()->registerCondition( this, deRichSys(), &DeRichPMT::initPMTQuantumEff );

  // Trigger first update
  sc = sc && ( updMgrSvc()->update(this));

  if ( sc.isFailure() ) { fatal() << "UMS updates failed" << endmsg; }


  return sc;
  
}
StatusCode DeRichPMT::getPMTParameters() {
  StatusCode  sc = StatusCode::SUCCESS;

  
  SmartDataPtr<DetectorElement> deRich1( dataSvc(), DeRichLocations::Rich1 );
  if ( !deRich1 )
  {
    error() << "Could not load DeRich1 for DePMT" << endmsg;
    return StatusCode::FAILURE;
  }

  m_PmtQELocation = deRich1->param<std::string> ("RichPmtQETableName");
  m_PmtAnodeXSize = deRich1->param<double> ("RichPmtAnodeXSize" );
  m_PmtAnodeYSize = deRich1->param<double> ("RichPmtAnodeYSize" );
  m_PmtAnodeZSize = deRich1->param<double> ("RichPmtAnodeZSize" );
  m_PmtAnodeLocationInPmt = deRich1->param<double> ("RichPmtSiliconDetectorLocalZlocation" );
  m_PmtPixelGap = deRich1->param<double> ( "RichPmtPixelGap" );
  m_PmtEffectivePixelXSize =  m_PmtAnodeXSize  + m_PmtPixelGap;
  m_PmtEffectivePixelYSize =  m_PmtAnodeYSize  + m_PmtPixelGap;
  m_PmtAnodeHalfThickness = m_PmtAnodeZSize/2.0;
  m_PmtNumPixCol =deRich1->param<int> ("RichPmtNumPixelCol");
  
  m_PmtNumPixRow = deRich1->param<int> ("RichPmtNumPixelRow");
  m_PmtQwZSize = deRich1->param<double>  ("RichPmtQuartzZSize"  );
  m_QwToAnodeZDist= deRich1->param<double> ( "RichPmtQWToSiMaxDist" );
  return sc;
  
}


StatusCode DeRichPMT::initPMTQuantumEff()
{

  SmartDataPtr<TabulatedProperty> pmtQuantumEffTabProp( dataSvc(), m_PmtQELocation );
  m_pdQuantumEffFunc = new Rich::TabulatedProperty1D( pmtQuantumEffTabProp );

return  StatusCode::SUCCESS; 
}
StatusCode DeRichPMT::updateGeometry()
{
StatusCode  sc = StatusCode::SUCCESS;


  return sc;   
}

StatusCode DeRichPMT::detectionPoint ( const double fracPixelCol,
                                       const double fracPixelRow,
                                       Gaudi::XYZPoint& detectPoint,
                                       const bool photoCathodeSide ) const
{ 
StatusCode  sc = StatusCode::SUCCESS;

Gaudi::XYZPoint aLocalHit= getAnodeHitCoordFromPixelNum( fracPixelCol,fracPixelRow );
double zPh = ( aLocalHit.z())+ m_QwToAnodeZDist +  m_PmtAnodeLocationInPmt ;
double zQwExt = zPh + m_PmtQwZSize;
 
Gaudi::XYZPoint aPhCathHit = Gaudi::XYZPoint (  aLocalHit.x(), aLocalHit.y(), zPh);
Gaudi::XYZPoint aQWExtHit =  Gaudi::XYZPoint (  aLocalHit.x(), aLocalHit.y(), zQwExt );
// detectPoint  =  geometry()->toGlobalMatrix() * aPhCathHit ; // not used since the
// output is expected to be in the PMT coord system.
// for now assume negligible refraction effect at the QW.
 
 detectPoint = photoCathodeSide ? aPhCathHit : aQWExtHit;
 

return sc;
  

}

StatusCode DeRichPMT::detectionPoint( const LHCb::RichSmartID smartID,
                                      Gaudi::XYZPoint& detectPoint,
                                      bool photoCathodeSide ) const
{
StatusCode   sc = StatusCode::SUCCESS;

 double aPixCol = (double) ( smartID.pixelCol())* 1.0;
 double aPixRow  = (double) ( smartID.pixelRow())* 1.0;
 Gaudi::XYZPoint aLocalHit= getAnodeHitCoordFromPixelNum( aPixCol,aPixRow );
 double zPh = ( aLocalHit.z())+ m_QwToAnodeZDist +  m_PmtAnodeLocationInPmt ;
 double zQwExt = zPh + m_PmtQwZSize;
 Gaudi::XYZPoint aPhCathHit = Gaudi::XYZPoint (  aLocalHit.x(), aLocalHit.y(), zPh);
 Gaudi::XYZPoint aQWExtHit =  Gaudi::XYZPoint (  aLocalHit.x(), aLocalHit.y(), zQwExt );
// for now assume negligible refraction effect at the QW.

 detectPoint  =  
  photoCathodeSide ? ( (geometry()->toGlobalMatrix()) * aPhCathHit) : ( (geometry()->toGlobalMatrix()) * aQWExtHit); 

  return sc;  
}


Gaudi::XYZPoint DeRichPMT::getAnodeHitCoordFromPixelNum( const double fracPixelCol, const double fracPixelRow ) const
{
     
 const double xh = ( fracPixelCol - (m_PmtNumPixCol - 1 ) *0.5 ) * m_PmtEffectivePixelXSize;
 const double yh = ( fracPixelRow - (m_PmtNumPixRow - 1 ) *0.5 ) * m_PmtEffectivePixelYSize;
 const double zh = m_PmtAnodeHalfThickness;

 return Gaudi::XYZPoint( xh,yh,zh);
 
}
