// $Id: MagFieldReader.cpp,v 1.1 2004-04-07 15:24:33 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

// local
#include "MagFieldReader.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MagFieldReader
//
// 08/05/2002 : Edgar De Oliveira
// 16/03/2004 : Gloria Corti, modified to fill ntuple
//-----------------------------------------------------------------------------
// Declaration of the Algorithm Factory
static const  AlgFactory<MagFieldReader>          s_factory ;
const        IAlgFactory& MagFieldReaderFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MagFieldReader::MagFieldReader( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_pIMF(0) 
{
  declareProperty("Zmin", m_zMin =  -500.0*mm);
  declareProperty("Zmax", m_zMax = 14000.0*mm);  
  declareProperty("Step", m_step =   100.0*mm);  
  declareProperty("Xmin", m_xMin =     0.0*mm);  
  declareProperty("Xmax", m_xMax =  4000.0*mm);
  declareProperty("Xmin", m_yMin =     0.0*mm);  
  declareProperty("Ymax", m_yMax =  4000.0*mm);
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MagFieldReader::initialize() {

  MsgStream msg(msgSvc(), name());
  
  msg << MSG::INFO << "FieldReader intialize() has been called" << endreq;
  StatusCode sc = service("MagneticFieldSvc", m_pIMF, true);
  if( !sc.isSuccess() ) {
    msg << MSG::FATAL << "Unable to open Magnetic field service" 
        << endreq;
    return StatusCode::FAILURE;
  }

  msg << MSG::DEBUG << " Book ntuple" << endreq;
  std::string ntname = "/NTUPLES/FILE1";
  NTupleFilePtr ntfile(ntupleSvc(), ntname);
  if( ntfile ) {
    StatusCode status = StatusCode::SUCCESS;
    ntname += "/"+(this->name())+"/10";
    NTuplePtr nt1(ntupleSvc(), ntname);
    if( !nt1 ) {
      nt1 = ntupleSvc()->book(ntname,CLID_ColumnWiseTuple,"Field");
      if( nt1 ) {
        if( status.isSuccess() ) nt1->addItem("x",  m_x );
        if( status.isSuccess() ) nt1->addItem("y",  m_y );
        if( status.isSuccess() ) nt1->addItem("z",  m_z );
        if( status.isSuccess() ) nt1->addItem("Bx", m_Bx );
        if( status.isSuccess() ) nt1->addItem("By", m_By );
        if( status.isSuccess() ) nt1->addItem("Bz", m_Bz );
        if( !status.isSuccess() ) {
          msg << MSG::ERROR << "Failure booking ntuples" << endreq;
          return StatusCode::FAILURE;
        } 
        m_ntuple = nt1;
      } else {  
        msg << MSG::ERROR << "Ntuple already exist" << endreq;
        return StatusCode::FAILURE;
      }
    }
  }   

  msg << MSG::INFO << "MagFieldReader initialized" << endreq;
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MagFieldReader::execute() {
  
  MsgStream  msg( msgSvc(), name() );

  msg << MSG::DEBUG << "==> Execute MagFieldReader" << endreq;
  
  StatusCode sc = this->readField();
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "Unable to execute MagFieldReader" 
        << endreq;
    return sc;
  }

  msg << MSG::INFO << "MagFieldReader executed" << endreq;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MagFieldReader::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize MagFieldReader" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
//=============================================================================
// The MagFieldReader print out info messages 
// with the field value at different locations.
//=============================================================================
StatusCode MagFieldReader::readField() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "m_pIMF = " << m_pIMF << endreq;

  HepVector3D B(0.0,0.0,0.0);
  for ( double z = m_zMin; z <= m_zMax; z += m_step ) {
    for( double y = m_yMin; y <= m_yMax; y += m_step ) {
      for( double x = m_xMin; x <= m_xMax; x += m_step ) {
        HepPoint3D P( x, y, z );
        // get field at point P
        m_pIMF->fieldVector( P, B );
        // fill ntuple
        m_x = P.x()/cm;
        m_y = P.y()/cm;
        m_z = P.z()/cm;
        m_Bx = B.x()/gauss;
        m_By = B.y()/gauss;
        m_Bz = B.z()/gauss;
        m_ntuple->write();
      }
    }
    HepPoint3D P0( 0.0, 0.0, z );
    m_pIMF->fieldVector( P0, B );
    msg << MSG::DEBUG << "Magnetic Field at ("
        << P0.x() << ", " << P0.y() << ", " << P0.z() << " ) = "
        << (B.x())/tesla << ", "
        << (B.y())/tesla << ", "
        << (B.z())/tesla << " Tesla " 
        << endreq;
  }

  // Return status code.
  return StatusCode::SUCCESS;
}
